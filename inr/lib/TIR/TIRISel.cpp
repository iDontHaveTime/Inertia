// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/MIR/MachineModule.h>
#include <inr/TIR/TIRISel.h>
#include <inr/Target/Triple.h>

namespace inr {

std::unique_ptr<MachineModule> TIRISel::select(const TIRModule* mod) {
    MachineModule* mmod = new MachineModule(mod->getName());

    Triple::TIRMatcherFunc matcher =
        usedLowering_.getTriple().getTIRMatchingFunc();

    for(const TIRFunction& tfunc : mod->getFunctions()) {
        MachineFunction* mfunc =
            mmod->newFunction(tfunc.getFunction()->getName());

        for(const TIRBlock& tblock : tfunc.getBlocks()) {
            MachineBlock* mblock =
                mfunc->newBlock(tblock.getBlock()->getName());
            for(const TIRInstruction& tinst : tblock.getInstructions()) {
                matcher(tinst, mblock);
            }
        }
    }

    return std::unique_ptr<MachineModule>(mmod);
}

} // namespace inr