// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/Support/Compiler.h>
#include <inr/TIR/TIRInstruction.h>
#include <inr/TIR/TIRLowering.h>
#include <inr/Target/CallingConv.h>
#include <inr/Target/Triple.h>

#include <unordered_map>

namespace inr {

void TIRLowering::emitStore(const Type* type, TIROperand* dest, TIROperand* src,
                            TIRBlock* block) {
    // Store must have the operands be:
    // dest: Reg/Mem
    // src: Rem/Mem/Imm
    // Since store isn't actually a memory-only store,
    // its more of a general copy instruction.
    // Also memory operands are exclusive, no memory to memory.
    // Also dest cannot be an immediate.

    inr_assert(!dest->isImm(), "Store destination cannot be an immediate.");

    // Now if they are both memory it would make sense to assert,
    // But in TIR its better to just emit multiple stores instead.

    if(dest->isMem() && src->isMem()) {
        TIROperand* newSrc = newVreg(block);

        emitStore(type, newSrc, src, block);

        src = newSrc;
    }

    // Now we have a store that could be:
    // reg, reg
    // reg, imm
    // reg, mem
    // mem, reg
    // mem, imm

    // Now mem and imm is an x86 thing, RISC doesn't allow that.

    if(dest->isMem() && src->isImm()) {
        if(!getDesc()->getInfo(TIRInstID::STORE).immToMemAllowed()) {
            TIROperand* newSrc = newVreg(block);

            emitStore(type, newSrc, src, block);

            src = newSrc;
        }
    }

    // Now we are left with a hopefully legal store.

    block->addInstruction(new TIRStore(block, type, dest, src));
}

constexpr uint8_t operator&(TIRInstInfo::OperandAllowed lhs,
                            TIRInstInfo::OperandAllowed rhs) noexcept {
    return uint8_t(lhs) & uint8_t(rhs);
}

bool TIRLowering::isAllowed(TIROperand* op,
                            TIRInstInfo::OperandAllowed allowed) noexcept {
    if(op->isReg() && allowed & TIRInstInfo::OperandAllowed::Reg) return true;
    if(op->isMem() && allowed & TIRInstInfo::OperandAllowed::Mem) return true;
    if(op->isImm() && allowed & TIRInstInfo::OperandAllowed::Imm) return true;
    return false;
}

TIROperand* TIRLowering::solveOperand(const Type* type, TIROperand* operand,
                                      TIRInstInfo::OperandAllowed allowed,
                                      TIRBlock* block) {
    if(isAllowed(operand, allowed)) return operand;

    if(operand->isImm()) {
        if(allowed & TIRInstInfo::OperandAllowed::Reg) {
            // prefer register
            TIROperand* newOp = newVreg(block);
            emitStore(type, newOp, operand, block);
            return newOp;
        }
        else if(allowed & TIRInstInfo::OperandAllowed::Mem) {
            // allocate stack
        }
    }
    else if(operand->isMem()) {
        if(allowed & TIRInstInfo::OperandAllowed::Reg) {
            TIROperand* newOp = newVreg(block);
            emitStore(type, newOp, operand, block);
            return newOp;
        }
        else {
            inr_notpossible("Cannot convert memory to immediate.");
        }
    }
    else if(operand->isReg()) {
        if(allowed & TIRInstInfo::OperandAllowed::Mem) {
            // allocate stack
        }
        else {
            inr_notpossible("Cannot convert register to immediate.");
        }
    }

    return operand;
}

void TIRLowering::solveOperands(const Type* type, TIROperand*& dest,
                                ncarrview<TIROperand*> srcs, TIRInstID tirInst,
                                TIRBlock* block) {
    // This function can and probably will modify incoming dest and operands.

    TIRInstInfo info = getDesc()->getInfo(tirInst);

    dest = solveOperand(type, dest, info.checkOperand(0), block);

    for(unsigned i = 0; i < srcs.size(); i++) {
        srcs[i] = solveOperand(type, srcs[i], info.checkOperand(i + 1), block);
    }

    unsigned memOps = dest->isMem() ? 1 : 0;
    for(TIROperand* op : srcs) {
        if(op->isMem()) {
            memOps++;
        }
    }

    // rather keep dest as mem.
    if(memOps > 1) {
        for(unsigned i = 0; i < srcs.size(); i++) {
            if(srcs[i]->isMem()) {
                if(info.checkOperand(i + 1) &
                   TIRInstInfo::OperandAllowed::Reg) {
                    TIROperand* newSrc = newVreg(block);
                    emitStore(type, newSrc, srcs[i], block);
                    srcs[i] = newSrc;
                    memOps--;
                }
                else {
                    inr_notpossible("Cannot convert memory to immediate.");
                }
            }
            if(memOps == 1) break;
        }
    }
}

void TIRLowering::emitArithmetic(TIRInstID op, const Type* type,
                                 TIROperand* dest, TIROperand* lhs,
                                 TIROperand* rhs, TIRBlock* block) {
    enum class ArithmeticOperation { ADDITION, SUBTRACTION };

    ArithmeticOperation finalOp;

    switch(op) {
        case TIRInstID::ADD_DEST_SRC:
        case TIRInstID::ADD_DEST_SRC_SRC:
            finalOp = ArithmeticOperation::ADDITION;
            break;
        case TIRInstID::SUB_DEST_SRC:
        case TIRInstID::SUB_DEST_SRC_SRC:
            finalOp = ArithmeticOperation::SUBTRACTION;
            break;
        default:
            inr_notpossible(
                "Instruction isn't arithmetic or is not supported.");
    }

    TIROperand* operands[2] = {lhs, rhs};

    if(finalOp == ArithmeticOperation::ADDITION ||
       finalOp == ArithmeticOperation::SUBTRACTION) {
        bool doubleSource = true;
        TIRInstID finalID = TIRInstID::TIR_INST_START;
        if(finalOp == ArithmeticOperation::ADDITION) {
            if(!getDesc()->getInfo(TIRInstID::ADD_DEST_SRC_SRC).getEnabled()) {
                if(getDesc()->getInfo(TIRInstID::ADD_DEST_SRC).getEnabled()) {
                    doubleSource = false;
                    finalID = TIRInstID::ADD_DEST_SRC;
                }
                else {
                    inr_notpossible("No add instruction present for target.");
                }
            }
            else {
                finalID = TIRInstID::ADD_DEST_SRC_SRC;
            }
        }
        else if(finalOp == ArithmeticOperation::SUBTRACTION) {
            if(!getDesc()->getInfo(TIRInstID::SUB_DEST_SRC_SRC).getEnabled()) {
                if(getDesc()->getInfo(TIRInstID::SUB_DEST_SRC).getEnabled()) {
                    doubleSource = false;
                    finalID = TIRInstID::SUB_DEST_SRC;
                }
                else {
                    inr_notpossible("No sub instruction present for target.");
                }
            }
            else {
                finalID = TIRInstID::SUB_DEST_SRC_SRC;
            }
        }

        TIRInstInfo info = getDesc()->getInfo(finalID);

        // This makes no sense as if lets say add had eax as an implicit dest.
        // This would mean this would be:
        // `add i32 eax, ?, ?` (instruction doesn't exist, but not the point).
        // But then its not really implicit, so implicit dest doesn't work in
        // add. Same thing applies for add dest, src. And any instruction that
        // expects dest really, but still to keep consistensy we will keep this
        // line.
        TIROperand* currentDest =
            info.isDestOverriden() ? &info.getImplicit()[0] : dest;

        if(doubleSource) {
            if(operands[0]->isImm() && operands[1]->isImm()) {
                TIROperand* newSrc = newVreg(block);
                emitStore(type, newSrc, operands[0], block);
                operands[0] = newSrc;
            }

            solveOperands(type, currentDest, operands, finalID, block);

            switch(finalOp) {
                case ArithmeticOperation::ADDITION:
                    block->addInstruction(new TIRAdd(block, type, currentDest,
                                                     operands[0], operands[1]));
                    break;
                case ArithmeticOperation::SUBTRACTION:
                    block->addInstruction(new TIRSub(block, type, currentDest,
                                                     operands[0], operands[1]));
                    break;
                default:
                    inr_notpossible("Already decided in the if above.");
            }
        }
        else {
            emitStore(type, currentDest, operands[0], block);

            solveOperands(type, currentDest,
                          ncarrview<TIROperand*>(operands + 1, 1), finalID,
                          block);

            switch(finalOp) {
                case ArithmeticOperation::ADDITION:
                    block->addInstruction(
                        new TIRAdd(block, type, currentDest, operands[1]));
                    break;
                case ArithmeticOperation::SUBTRACTION:
                    block->addInstruction(
                        new TIRSub(block, type, currentDest, operands[1]));
                    break;
                default:
                    inr_notpossible("Already decided in the if above.");
            }
        }

        if(currentDest != dest) {
            emitStore(type, dest, currentDest, block);
        }
    }
}

void TIRLowering::emitAdd(const Type* type, TIROperand* dest, TIROperand* lhs,
                          TIROperand* rhs, TIRBlock* block) {
    emitArithmetic(TIRInstID::ADD_DEST_SRC, type, dest, lhs, rhs, block);
}

void TIRLowering::emitSub(const Type* type, TIROperand* dest, TIROperand* lhs,
                          TIROperand* rhs, TIRBlock* block) {
    emitArithmetic(TIRInstID::SUB_DEST_SRC, type, dest, lhs, rhs, block);
}

void TIRLowering::lowerSSAAdd(const BinaryInst* inst, TIRBlock* block) {
    TIROperand* dest = newVreg(block);
    emitAdd(inst->getType(), dest, operandMap_[inst->getOperand(0)],
            operandMap_[inst->getOperand(1)], block);
    mapOperand(inst, dest);
}

void TIRLowering::lowerSSABinaryInst(const BinaryInst* inst, TIRBlock* block) {
    if(inst->getID() == Instruction::InstructionID::ADD)
        lowerSSAAdd(inst, block);
}

void TIRLowering::lowerSSARet(const Instruction* inst, TIRBlock* block) {
    if(!inst->getType()->isVoid()) {
        TIROperand* src = operandMap_[inst->getOperand(0)];

        CCStateGeneric state(false, triple_.getRegisterInfo(),
                             triple_.getRegisterInfo()->getRegs());
        state.analyzeReturn(
            inst->getType(),
            triple_.getCCRet(block->getBlock()->getParent()->getCC()));

        for(const CCAssign& assign : state.getAssigns()) {
            if(assign.isReg()) {
                TIROperand* dest = newOperand(assign.getDestReg());
                emitStore(inst->getType(), dest, src, block);
            }
        }
    }

    block->addInstruction(new TIRRet(block, inst->getType()));
}

void TIRLowering::lowerSSAAlloca(const AllocaInst* inst, TIRBlock* block) {}

void TIRLowering::lowerSSAInstruction(const Instruction* inst, TIRBlock* block,
                                      CCStateGeneric& state) {
    for(const Value* op : inst->getOperands()) {
        lowerSSAValue(op, block, state);
    }

    switch(inst->getID()) {
        case Instruction::InstructionID::RETURN:
            lowerSSARet(inst, block);
            break;
        case Instruction::InstructionID::ADD:
            lowerSSABinaryInst((const BinaryInst*)inst, block);
            break;
        case Instruction::InstructionID::ALLOCA:
            lowerSSAAlloca((const AllocaInst*)inst, block);
            break;
        case Instruction::InstructionID::LOAD:
            break;
        case Instruction::InstructionID::STORE:
            break;
    }
}

void TIRLowering::lowerSSAValue(const Value* val, TIRBlock* block,
                                CCStateGeneric& state) {
    if(operandMap_.count(val)) return;

    switch(val->getValueID()) {
        case Value::ValueID::Argument: {
            const Argument* arg = (const Argument*)val;
            const CCAssign& assign = state.getAssigns()[arg->getIndex()];
            if(assign.isReg()) {
                TIROperand* dest = newVreg(block);

                emitStore(val->getType(), dest, newOperand(assign.getDestReg()),
                          block);

                mapOperand(val, dest);
            }
        } break;
        case Value::ValueID::ConstantInt:
            mapOperand(val, newOperand((const ConstantInt*)val));
            break;
        case Value::ValueID::Instruction:
            lowerSSAInstruction((const Instruction*)val, block, state);
            break;
        case Value::ValueID::Function:
            inr_notpossible("Function is an unsupported operand.");
        case Value::ValueID::Block:
            inr_notpossible("Block is an unsupported operand.");
    }
}

void TIRLowering::lowerSSABlock(const Block& block, TIRBlock* tblock,
                                CCStateGeneric& state) {
    for(const Instruction& inst : block.getInstructions()) {
        lowerSSAValue(&inst, tblock, state);
    }
}

void TIRLowering::lowerSSAFunction(const Function& func, TIRModule* mod) {
    TIRFunction* tfunc = mod->newFunction(&func);
    CCStateGeneric state(false, triple_.getRegisterInfo(),
                         triple_.getRegisterInfo()->getRegs());

    state.analyzeArgs(func.getType()->getArgs(),
                      triple_.getCCArgs(func.getCC()));
    for(const Block& block : func.getBlocks()) {
        lowerSSABlock(block, tfunc->newBlock(&block), state);
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
        case TIRInstID::SUB_DEST_SRC:
        case TIRInstID::SUB_DEST_SRC_SRC:
            os << "sub";
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