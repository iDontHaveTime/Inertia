// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/IR/Block.h>
#include <inr/IR/Function.h>
#include <inr/IR/Instruction.h>

namespace inr {

raw_stream& operator<<(raw_stream& os, const Instruction& ins) {
    switch(ins.getID()) {
        case Instruction::InstructionID::RETURN:
            if(ins.getNumOperands()) {
                return os << "ret " << *ins.getOperand(0);
            }
            else {
                return os << "ret void";
            }
        case Instruction::InstructionID::ADD:
            return os << '%' << ins.getName() << " = add " << *ins.getType()
                      << ' ' << *ins.getOperand(0) << ", "
                      << *ins.getOperand(1);
        default:
            return os;
    }
}

void Instruction::append(Block* to, Instruction* ins) {
    to->instructions_.push_back(ins);
}

ReturnInst::ReturnInst(Value* retVal, Block* parent) :
    Instruction(Instruction::InstructionID::RETURN,
                retVal ? retVal->getType()
                       : parent->getParent()->getType()->getReturn(),
                parent,
                retVal ? std::vector<Value*>{retVal} : std::vector<Value*>{},
                "ret") {}

} // namespace inr