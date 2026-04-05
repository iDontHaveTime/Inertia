// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TARGET_ASMPRINTER_H
#define INERTIA_TARGET_ASMPRINTER_H

/// @file Target/AsmPrinter.h
/// @brief Prints out textual assembly.

#include <inr/MIR/MachineBlock.h>
#include <inr/MIR/MachineFunction.h>
#include <inr/MIR/MachineInst.h>
#include <inr/MIR/MachineModule.h>
#include <inr/MIR/Register.h>
#include <inr/Support/Stream.h>
#include <inr/Target/Triple.h>

namespace inr {

class AsmPrinter {
    const RegisterInfo* regInfo_;

public:
    AsmPrinter(Triple triple) noexcept : regInfo_(triple.getRegisterInfo()) {}

    virtual ~AsmPrinter() noexcept = default;

    void emit(raw_stream& os, const MachineModule* mmod) const;
    void emitMF(raw_stream& os, const MachineFunction& mf) const;
    void emitMB(raw_stream& os, const MachineBlock& mb) const;

    virtual void emitMI(raw_stream& os, const MachineInst& mi) const = 0;

    const RegisterInfo* getRegisterInfo() const noexcept {
        return regInfo_;
    }
};

} // namespace inr

#endif // INERTIA_TARGET_ASMPRINTER_H
