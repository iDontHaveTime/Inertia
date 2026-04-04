// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/DAG/DAGNode.h>
#include <inr/DAG/SelectionDAG.h>
#include <inr/IR/Argument.h>
#include <inr/IR/Block.h>
#include <inr/IR/Constant.h>
#include <inr/IR/Function.h>
#include <inr/IR/Instruction.h>
#include <inr/IR/Value.h>
#include <inr/MIR/MachineBlock.h>
#include <inr/MIR/MachineFunction.h>
#include <inr/MIR/MachineModule.h>
#include <inr/Target/CallingConv.h>
#include <inr/Target/ISel.h>
#include <inr/Target/Triple.h>

namespace inr {

MachineModule* ISel::lower(const Module* module) {
    MachineModule* mm = new MachineModule(module->getName());

    for(const Function& func : module->getFunctions()) {
        mm->addFunction(lowerFunction(func, mm));
    }

    return mm;
}
MachineFunction* ISel::lowerFunction(const Function& function,
                                     MachineModule* mm) {
    MachineFunction* mfunc = new MachineFunction(function.getName(), mm);
    CCState state(mfunc, false, getRegisterInfo(),
                  getRegisterInfo()->getRegs());

    // override if needed.
    CallingConv cc = CallingConv::C;

    state.analyzeArgs(function.getType()->getArgs(), getTriple().getCCArgs(cc));

    for(const Block& block : function.getBlocks()) {
        SelectionDAG dag;
        lowerBlock(block, mfunc, dag, state, cc);
    }
    return mfunc;
}
MachineBlock* ISel::lowerBlock(const Block& block, MachineFunction* mf,
                               SelectionDAG& selDag, CCState& state,
                               CallingConv cc) {
    MachineBlock* mb = mf->newBlock(block.getName());
    ValueMap map;

    for(const Instruction& inst : block.getInstructions()) {
        for(Value* op : inst.getOperands()) {
            lowerValue(op, selDag, state, map);
        }

        switch(inst.getID()) {
            // RETURN
            case Instruction::InstructionID::RETURN:
                lowerReturn(inst, selDag, cc, map, mb);
                break;
            // BINARY OP
            case Instruction::InstructionID::ADD: {
                DAGNode* lhs = map[inst.getOperand(0)];
                DAGNode* rhs = map[inst.getOperand(1)];
                DAGNode* node = selDag.createAdd(inst.getType(), lhs, rhs);
                map[&inst] = node;
                matchEmit(node, mb);
                break;
            }
        }
    }

    return mb;
}
void ISel::lowerValue(const Value* val, SelectionDAG& selDag, CCState& state,
                      ValueMap& map) {
    if(map.count(val)) return;

    switch(val->getValueID()) {
        case Value::ValueID::Argument: {
            const Argument* arg = (const Argument*)val;
            const CCAssign& assign = state.getAssigns()[arg->getIndex()];
            if(assign.isReg()) {
                map[val] = selDag.createCopyFromReg(assign.getDestReg(),
                                                    assign.getDestType());
            }
            break;
        }
        case Value::ValueID::ConstantInt:
            map[val] = selDag.createConstant((const ConstantInt*)val);
            break;
        case Value::ValueID::Instruction:
            [[fallthrough]];
        case Value::ValueID::Function:
            [[fallthrough]];
        case Value::ValueID::Block:
            break;
    }
}

} // namespace inr