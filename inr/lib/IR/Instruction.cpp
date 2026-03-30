// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/IR/Block.h>
#include <inr/IR/Function.h>
#include <inr/IR/Instruction.h>

namespace inr {

sview Instruction::getInstructionIDStr(InstructionID id) {
    switch(id) {
        case InstructionID::RETURN:
            return "ret";
        case InstructionID::ADD:
            return "add";
    }
}

raw_stream& operator<<(raw_stream& os, const Instruction& ins) {
    switch(ins.getID()) {
        case Instruction::InstructionID::RETURN:
            os << Instruction::getInstructionIDStr(ins.getID());
            if(ins.getNumOperands()) {
                return os << *ins.getType() << ' ' << *ins.getOperand(0);
            }
            else {
                return os << "void";
            }
        case Instruction::InstructionID::ADD:
            return os << '%' << ins.getName() << " = "
                      << Instruction::getInstructionIDStr(ins.getID()) << ' '
                      << *ins.getType() << ' ' << *ins.getOperand(0) << ", "
                      << *ins.getOperand(1);
        default:
            return os;
    }
}

void Instruction::append(Block* to, Instruction* ins) {
    to->instructions_.push_back(ins);
}

ReturnInst::ReturnInst(Value* retVal, Block* parent) :
    TermInst(InstructionID::RETURN,
             retVal ? retVal->getType()
                    : parent->getParent()->getType()->getReturn(),
             parent, retVal ? ivec<Value*, 3>{retVal} : ivec<Value*, 3>{},
             "ret") {}

} // namespace inr