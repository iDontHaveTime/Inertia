// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/MIR/MachineBlock.h>
#include <inr/MIR/MachineInst.h>

namespace inr {
MachineInst* MachineInst::create(uint32_t op, MachineBlock* parent,
                                 ivec<MachineOperand, 3> operands) {
    return parent->push(new MachineInst(op, parent, operands));
}
} // namespace inr