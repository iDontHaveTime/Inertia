#include <inr/IR/Context.h>
#include <inr/IR/Function.h>
#include <inr/ISel/TargetTree.h>
#include <inr/ISel/x86/LinearISelPass.h>
#include <inr/MIR/MachineBlock.h>
#include <inr/MIR/MachineFunction.h>

/// @file lib/ISel/x86/LinearISelPass.cpp
/// @brief This file implements the linear instruction selector pass for x86.
///
/// This is the main way to do instruction selection on x86.

namespace inr::x86 {

void lowerFunction(const InrContext&, Function& func, MachineFunction& mfunc,
                   TargetTree* tt) {
    for(Block& b : func.getBlocks()) {
        MachineBlock* block = mfunc.newBlock(b.getName());
        /// Spill args to stack.
    }
}

void LinearISelPass::run(const InrContext& ctx, Module& mod,
                         MachineModule& mmod, TargetTree* tt) {
    /// x86 Linear ISel Pass
    for(Function& f : mod.getFunctions()) {
        lowerFunction(ctx, f, *mmod.newFunction(f.getName()), tt);
    }
}

} // namespace inr::x86