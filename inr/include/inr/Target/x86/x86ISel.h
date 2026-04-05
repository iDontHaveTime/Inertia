// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_X86_X86ISEL_H
#define INERTIA_X86_X86ISEL_H

/// @file Target/x86/x86ISel.h
/// @brief Contains the x86 ISel.

#include <inr/Target/ISel.h>
#include <inr/Target/Triple.h>

#include <stdexcept>

namespace inr::x86 {

class x86ISel : public ISel {
    void lowerCall(const Instruction& inst, SelectionDAG& dag,
                   MachineBlock* mb) override;

    void lowerReturn(const Instruction& inst, SelectionDAG& dag, CallingConv cc,
                     ValueMap& map, MachineFunction* mf) override;

    void matchEmit(DAGNode* dag, MachineBlock* block) override;

public:
    x86ISel(Triple triple) : ISel(triple) {
        if(triple.getArch() != Triple::Arch::x86_64)
            throw std::runtime_error("Triple is not x86 in x86ISel.");
    }
};

} // namespace inr::x86

#endif // INERTIA_X86_X86ISEL_H
