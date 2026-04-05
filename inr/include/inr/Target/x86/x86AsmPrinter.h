// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_X86_X86ASMPRINTER_H
#define INERTIA_X86_X86ASMPRINTER_H

/// @file Target/x86/x86AsmPrinter.h
/// @brief AsmPrinter for x86.

#include <inr/Target/AsmPrinter.h>

namespace inr::x86 {

class x86AsmPrinter : public AsmPrinter {
public:
    x86AsmPrinter(Triple triple) noexcept : AsmPrinter(triple) {}

    void emitMI(raw_stream& os, const MachineInst& mi) const override;
};

} // namespace inr::x86

#endif // INERTIA_X86_X86ASMPRINTER_H
