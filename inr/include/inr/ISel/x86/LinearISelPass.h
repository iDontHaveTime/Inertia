// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_X86_LINEARISELPASS_H
#define INERTIA_X86_LINEARISELPASS_H

/// @file ISel/x86/LinearISelPass.h
/// @brief Contains the x86 linear ISel pass class.

#include <inr/PassManager.h>

namespace inr::x86 {

class LinearISelPass : public ISelPass {
    void run(const InrContext& ctx, Module& mod, MachineModule& mmod,
             const TargetTree* tt) override;
};

} // namespace inr::x86

#endif // INERTIA_X86_LINEARISELPASS_H
