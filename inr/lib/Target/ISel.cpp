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

void printDAG(DAGValue root, std::unordered_set<DAGNode*>& visited,
              int depth = 0) {
    inr::outs().indent(depth) << (DAGType)root.getNode()->getNodeType();

    if(visited.count(root.getNode())) {
        inr::outs() << " (seen)";
    }

    inr::outs() << '\n';
    visited.insert(root.getNode());

    for(DAGNode* op : root.getNode()->getOperands()) {
        printDAG(op, visited, depth + 2);
    }
}

void ISel::buildForBlock(const Block& block, SelectionDAG& selDag,
                         CCState& state, CallingConv cc, MachineBlock* mb,
                         ValueMap& map) {
    for(const Instruction& inst : block.getInstructions()) {
        lowerValue(&inst, selDag, state, cc, mb, map);
    }
}

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
        std::unordered_set<DAGNode*> v;
        // printDAG(dag.getRoot(), v);
    }
    return mfunc;
}

MachineBlock* ISel::lowerBlock(const Block& block, MachineFunction* mf,
                               SelectionDAG& selDag, CCState& state,
                               CallingConv cc) {
    MachineBlock* mb = mf->newBlock(block.getName());
    ValueMap map;

    buildForBlock(block, selDag, state, cc, mb, map);

    DAGValue root = selDag.getRoot();
    std::unordered_set<DAGNode*> visited;
    postOrderEmit(root.getNode(), mb, visited);

    return mb;
}

void ISel::postOrderEmit(DAGNode* node, MachineBlock* mb,
                         std::unordered_set<DAGNode*>& visited) {
    if(!node || visited.count(node)) return;
    visited.insert(node);

    for(DAGNode*& op : node->getOperands()) {
        postOrderEmit(op, mb, visited);
        if(op->getNodeType() == (uint32_t)DAGType::CopyFromReg) {
            op = op->getOperand(0);
        }
    }

    matchEmit(node, mb);
}

void ISel::lowerValue(const Value* val, SelectionDAG& selDag, CCState& state,
                      CallingConv cc, MachineBlock* mb, ValueMap& map) {
    if(map.count(val)) return;

    switch(val->getValueID()) {
        case Value::ValueID::Argument:
            if(const CCAssign& assign =
                   state.getAssigns()[((const Argument*)val)->getIndex()];
               assign.isReg()) {
                map[val] = selDag.createCopyFromReg(
                    selDag.createRegister(state.getMFunc()->newVregValue(val),
                                          val->getType()),
                    assign.getDestReg(), assign.getDestType());
            }
            break;
        case Value::ValueID::ConstantInt:
            map[val] = selDag.createConstant((const ConstantInt*)val);
            break;
        case Value::ValueID::Instruction: {
            const Instruction* inst = (const Instruction*)val;
            for(unsigned i = 0; i < inst->getNumOperands(); i++) {
                lowerValue(inst->getOperand(i), selDag, state, cc, mb, map);
            }

            switch(inst->getID()) {
                case Instruction::InstructionID::RETURN:
                    lowerReturn(*inst, selDag, cc, map, mb->getParent());
                    break;
                case Instruction::InstructionID::ADD: {
                    DAGValue lhs = map[inst->getOperand(0)],
                             rhs = map[inst->getOperand(1)];
                    DAGValue addInst =
                        selDag.createAdd(inst->getType(), lhs, rhs);
                    map[inst] = selDag.createCopyToReg(
                        addInst, state.getMFunc()->newVregValue(inst),
                        inst->getType());
                    break;
                }
            }
            break;
        }
        case Value::ValueID::Function:
            [[fallthrough]];
        case Value::ValueID::Block:
            break;
    }
}

} // namespace inr