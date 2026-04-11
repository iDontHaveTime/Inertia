// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TARGET_ASMPRINTER_H
#define INERTIA_TARGET_ASMPRINTER_H

/// @file Target/AsmPrinter.h
/// @brief Prints out textual assembly.

#include <inr/IR/Global.h>
#include <inr/MIR/MachineBlock.h>
#include <inr/MIR/MachineFunction.h>
#include <inr/MIR/MachineInst.h>
#include <inr/MIR/MachineModule.h>
#include <inr/MIR/Register.h>
#include <inr/Support/Stream.h>
#include <inr/Target/Triple.h>

namespace inr {

class AsmPrinter {
    Triple triple_;
    const RegisterInfo* regInfo_;

public:
    enum class ELFTypeDirective { NoType, Function, Object, TLSObject, Common };

    enum class SectionType { Text, BSS, Data, Rodata };

    AsmPrinter(Triple triple) noexcept :
        triple_(triple), regInfo_(triple.getRegisterInfo()) {}

    virtual ~AsmPrinter() noexcept = default;

    void emit(raw_stream& os, const MachineModule* mmod) const;
    void emitMF(raw_stream& os, const MachineFunction& mf) const;
    void emitMB(raw_stream& os, const MachineBlock& mb) const;

    virtual void emitMI(raw_stream& os, const MachineInst& mi) const = 0;

    void emitELFTypeDirective(raw_stream& os, ELFTypeDirective type,
                              sview label) const;
    void emitLinkage(raw_stream& os, Global::Linkage linkage,
                     sview label) const;

    void emitCommon(raw_stream& os, sview label, size_t size,
                    Alignment align) const;

    void emitSection(raw_stream& os, SectionType sect) const;
    void emitAlignment(raw_stream& os, Alignment alignment) const;

    const RegisterInfo* getRegisterInfo() const noexcept {
        return regInfo_;
    }

    Triple getTriple() const noexcept {
        return triple_;
    }
};

} // namespace inr

#endif // INERTIA_TARGET_ASMPRINTER_H
