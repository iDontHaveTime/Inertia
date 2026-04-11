// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/TIR/InstIt.h>
#include <inr/TIR/StoreCleanup.h>
#include <inr/TIR/TIRInstruction.h>

namespace inr {

void StoreCleanup::run(TIRModule* mod) {
    for(TIRFunction& func : mod->getFunctions()) {
        // We should remove:
        // store reg, reg
        // Where reg == reg
        for(TIRBlock& block : func.getBlocks()) {
            for(auto it = block.getInstructions().begin();
                it != block.getInstructions().end();) {
                TIRInstruction& inst = *it;
                if(inst.getInstID() == TIRInstID::STORE &&
                   *inst.getOperand(0) == *inst.getOperand(1)) {
                    it = block.getInstructions().erase_delete(it);
                }
                else {
                    ++it;
                }
            }
        }
    }
}

} // namespace inr