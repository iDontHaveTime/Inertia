// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/TIR/TIRLowering.h>
#include <inr/Target/CallingConv.h>
#include <inr/Target/Triple.h>

#include <memory>
#include <unordered_map>

namespace inr {

void TIRLowering::lowerSSAAdd(const BinaryInst* inst, TIRBlock* block) {
    TIROperand* dest = mapOperand(
        inst, newOperand(
                  Register::createVirtual(block->getParent()->allocateVreg())));
    TIROperand* lhs = operandMap_[inst->getOperand(0)];
    TIROperand* rhs = operandMap_[inst->getOperand(1)];
    if(getDesc()->enabledInst(TIRInstID::ADD_DEST_SRC_SRC)) {
        block->addInstruction(
            new TIRAdd(block, inst->getType(), dest, lhs, rhs));
    }
    else if(getDesc()->enabledInst(TIRInstID::ADD_DEST_SRC)) {
        block->addInstruction(new TIRStore(block, inst->getType(), dest, lhs));
        block->addInstruction(new TIRAdd(block, inst->getType(), dest, rhs));
    }
    else throw std::runtime_error("No add instruction present for target.");
}

void TIRLowering::lowerSSABinaryInst(const BinaryInst* inst, TIRBlock* block) {
    if(inst->getID() == Instruction::InstructionID::ADD)
        lowerSSAAdd(inst, block);
}

void TIRLowering::lowerSSARet(const Instruction* inst, TIRBlock* block,
                              CallingConv cc) {
    if(!inst->getType()->isVoid()) {
        TIROperand* src = operandMap_[inst->getOperand(0)];

        CCStateGeneric state(false, triple_.getRegisterInfo(),
                             triple_.getRegisterInfo()->getRegs());
        state.analyzeReturn(inst->getType(), triple_.getCCRet(cc));

        for(const CCAssign& assign : state.getAssigns()) {
            if(assign.isReg()) {
                TIROperand* dest = newOperand(assign.getDestReg());
                block->addInstruction(
                    new TIRStore(block, inst->getType(), dest, src));
            }
        }
    }

    block->addInstruction(new TIRRet(block, inst->getType()));
}

void TIRLowering::lowerSSAInstruction(const Instruction* inst, TIRBlock* block,
                                      CCStateGeneric& state, CallingConv cc) {
    for(const Value* op : inst->getOperands()) {
        lowerSSAValue(op, block, state, cc);
    }

    switch(inst->getID()) {
        case Instruction::InstructionID::RETURN:
            lowerSSARet(inst, block, cc);
            break;
        case Instruction::InstructionID::ADD:
            lowerSSABinaryInst((const BinaryInst*)inst, block);
            break;
    }
}

void TIRLowering::lowerSSAValue(const Value* val, TIRBlock* block,
                                CCStateGeneric& state, CallingConv cc) {
    if(operandMap_.count(val)) return;

    switch(val->getValueID()) {
        case Value::ValueID::Argument: {
            const Argument* arg = (const Argument*)val;
            const CCAssign& assign = state.getAssigns()[arg->getIndex()];
            if(assign.isReg()) {
                TIROperand* dest =
                    mapOperand(val, newOperand(Register::createVirtual(
                                        block->getParent()->allocateVreg())));
                block->addInstruction(
                    new TIRStore(block, val->getType(), dest,
                                 newOperand(assign.getDestReg())));
            }
        } break;
        case Value::ValueID::ConstantInt:
            mapOperand(val, newOperand((const ConstantInt*)val));
            break;
        case Value::ValueID::Instruction:
            lowerSSAInstruction((const Instruction*)val, block, state, cc);
            break;
        case Value::ValueID::Function:
            throw std::runtime_error("Function is an unsupported operand.");
        case Value::ValueID::Block:
            throw std::runtime_error("Block is an unsupported operand.");
    }
}

void TIRLowering::lowerSSABlock(const Block& block, TIRBlock* tblock,
                                CCStateGeneric& state, CallingConv cc) {
    for(const Instruction& inst : block.getInstructions()) {
        lowerSSAValue(&inst, tblock, state, cc);
    }
}

void TIRLowering::lowerSSAFunction(const Function& func, TIRModule* mod) {
    TIRFunction* tfunc = mod->newFunction(&func);
    CallingConv cc = CallingConv::C;
    CCStateGeneric state(false, triple_.getRegisterInfo(),
                         triple_.getRegisterInfo()->getRegs());

    state.analyzeArgs(func.getType()->getArgs(), triple_.getCCArgs(cc));
    for(const Block& block : func.getBlocks()) {
        lowerSSABlock(block, tfunc->newBlock(&block), state, cc);
    }
}

std::unique_ptr<TIRModule> TIRLowering::lowerSSA(const Module* mod) {
    if(!mod) return {};
    operandMap_.clear();
    TIRModule* tirmod = new TIRModule(mod);

    for(const Function& func : mod->getFunctions()) {
        lowerSSAFunction(func, tirmod);
    }

    return std::unique_ptr<TIRModule>(tirmod);
}

static inline void printTIROp(
    const TIROperand& op, raw_stream& os, const RegisterInfo* regInfo,
    std::unordered_map<const TIROperand*, uint32_t>& visited, uint32_t& count) {
    if(visited.count(&op)) return;
    uint32_t num = count;
    visited[&op] = count++;

    os << "\t%" << num << " = ";

    if(op.isReg()) {
        Register reg = op.getReg();
        if(reg.isPhysical()) {
            os << "reg " << regInfo->getName(reg);
        }
        else if(reg.isVirtual()) {
            os << "vreg " << reg.getIndex();
        }
        else {
            os << '?';
        }
    }
    else if(op.isImm()) {
        os << "imm " << op.getImm()->getValue();
    }

    os << '\n';
}

static inline void printTIRInst(
    const TIRInstruction& tinst, raw_stream& os, const RegisterInfo* regInfo,
    std::unordered_map<const TIROperand*, uint32_t>& visited, uint32_t& count) {
    for(const TIROperand* op : tinst.getOperands()) {
        printTIROp(*op, os, regInfo, visited, count);
    }

    bool needType = true;

    os << '\t';

    switch(tinst.getInstID()) {
        case TIRInstID::STORE:
            os << "store";
            break;
        case TIRInstID::ADD_DEST_SRC:
        case TIRInstID::ADD_DEST_SRC_SRC:
            os << "add";
            break;
        case TIRInstID::RET:
            needType = false;
            os << "ret";
            break;
        case TIRInstID::TIR_INST_START:
        case TIRInstID::TIR_INST_END:
            break;
    }

    if(needType) os << ' ' << *tinst.getType() << ' ';

    for(const TIROperand* op : tinst.getOperands()) {
        if(op != tinst.getOperands().front()) {
            os << ", ";
        }

        os << '%' << visited[op];
    }

    os << '\n';
}

void TIRLowering::print(const TIRModule* mod, raw_stream& os) const {
    if(!mod) return;
    const RegisterInfo* regInfo = triple_.getRegisterInfo();
    std::unordered_map<const TIROperand*, uint32_t> visited;
    uint32_t count = 0;
    os << "; TIRModule = " << mod->getName() << '\n';
    os << "; target = " << triple_ << '\n';

    for(const TIRFunction& func : mod->getFunctions()) {
        os << "proc " << *func.getFunction()->getType()->getReturn() << " @"
           << func.getFunction()->getName() << '(';
        for(const Argument& arg : func.getFunction()->getArgs()) {
            if(&arg != func.getFunction()->getArg(0)) {
                os << ", ";
            }

            os << *arg.getType() << " %" << arg.getName();
        }

        os << ") {\n";

        for(const TIRBlock& block : func.getBlocks()) {
            os << block.getBlock()->getName() << ":\n";
            for(const TIRInstruction& inst : block.getInstructions()) {
                printTIRInst(inst, os, regInfo, visited, count);
            }
        }

        os << "}\n";
    }
}

} // namespace inr