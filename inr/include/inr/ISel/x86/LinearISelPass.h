#ifndef INERTIA_X86_LINEARISELPASS_H
#define INERTIA_X86_LINEARISELPASS_H

/// @file ISel/x86/LinearISelPass.h
/// @brief Contains the x86 linear ISel pass class.

#include <inr/PassManager.h>

namespace inr::x86 {

class LinearISelPass : public ISelPass {
    void run(const InrContext& ctx, Module& mod, MachineModule& mmod,
             TargetTree* tt) override;
};

} // namespace inr::x86

#endif // INERTIA_X86_LINEARISELPASS_H
