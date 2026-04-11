// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/MIR/MachineBlock.h>
#include <inr/MIR/MachineInst.h>
#include <inr/MIR/MachineOperand.h>
#include <inr/TIR/TIRInstruction.h>

namespace inr {
MachineInst* MachineInst::create(uint32_t op, MachineBlock* parent,
                                 ivec<MachineOperand, 3> operands) {
    return parent->push(new MachineInst(op, parent, operands));
}

MachineInst* MachineInst::create(uint32_t op, MachineBlock* parent,
                                 ivec<const TIROperand*, 3> top) {
    ivec<MachineOperand, 3> mops;

    for(const TIROperand* tiroperand : top) {
        if(tiroperand->isImm()) {
            mops.emplace_back(MachineOperand::createImm(tiroperand->getImm()));
        }
        else if(tiroperand->isReg()) {
            mops.emplace_back(MachineOperand::createReg(tiroperand->getReg()));
        }
        else if(tiroperand->isMemReg()) {
            mops.emplace_back(
                MachineOperand::createMem(tiroperand->getMemReg()));
        }
        else if(tiroperand->isGlobal()) {
            mops.emplace_back(
                MachineOperand::createGlobal(tiroperand->getGlobal()));
        }
    }

    return parent->push(new MachineInst(op, parent, std::move(mops)));
}

} // namespace inr