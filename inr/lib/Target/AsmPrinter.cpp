// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/Target/AsmPrinter.h>

namespace inr {

void AsmPrinter::emit(raw_stream& os, const MachineModule* mmod) const {
    for(const MachineFunction& mf : mmod->getFunctions()) {
        emitMF(os, mf);
    }
}

void AsmPrinter::emitMF(raw_stream& os, const MachineFunction& mf) const {
    os << mf.getName() << ":\n";
    for(const MachineBlock& mb : mf.getBlocks()) {
        emitMB(os, mb);
    }
}

void AsmPrinter::emitMB(raw_stream& os, const MachineBlock& mb) const {
    for(const MachineInst& mi : mb.getInstructions()) {
        emitMI(os, mi);
    }
}

} // namespace inr