// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/IR/Function.h>
#include <inr/Target/AsmPrinter.h>

namespace inr {

void AsmPrinter::emit(raw_stream& os, const MachineModule* mmod) const {
    emitSection(os, SectionType::Text);
    for(const MachineFunction& mf : mmod->getFunctions()) {
        emitMF(os, mf);
    }
}

void AsmPrinter::emitMF(raw_stream& os, const MachineFunction& mf) const {
    emitLinkage(os, mf.getFunction()->getLinkage(),
                mf.getFunction()->getName());

    Triple::FileType ft = triple_.getFileType();

    if(ft == Triple::FileType::ELF)
        emitELFTypeDirective(os, ELFTypeDirective::Function,
                             mf.getFunction()->getName());

    emitAlignment(os, mf.getFunction()->getAlignment());

    os << mf.getFunction()->getName() << ":\n";

    for(const MachineBlock& mb : mf.getBlocks()) {
        emitMB(os, mb);
    }
}

void AsmPrinter::emitMB(raw_stream& os, const MachineBlock& mb) const {
    for(const MachineInst& mi : mb.getInstructions()) {
        emitMI(os, mi);
    }
}

void AsmPrinter::emitCommon(raw_stream& os, sview label, size_t size,
                            Alignment align) const {
    os << ".comm " << label << ", " << size << ", "
       << (triple_.getFileType() == Triple::FileType::ELF ? align.getAlignment()
                                                          : align.getPow2());
}

void AsmPrinter::emitLinkage(raw_stream& os, Global::Linkage linkage,
                             sview label) const {
    switch(linkage) {
        case Global::External:
            os << ".globl " << label;
            break;
        case Global::Internal:
            break;
        case Global::Weak:
            os << ".weak " << label;
            break;
        default:
            return;
    }
    os << '\n';
}

void AsmPrinter::emitELFTypeDirective(raw_stream& os, ELFTypeDirective type,
                                      sview label) const {
    os << ".type " << label << ", @";
    switch(type) {
        case ELFTypeDirective::NoType:
            os << "notype";
            break;
        case ELFTypeDirective::Function:
            os << "function";
            break;
        case ELFTypeDirective::Object:
            os << "object";
            break;
        case ELFTypeDirective::TLSObject:
            os << "tls_object";
            break;
        case ELFTypeDirective::Common:
            os << "common";
            break;
    }
    os << '\n';
}

void AsmPrinter::emitSection(raw_stream& os, SectionType sect) const {
    os << ".section ";
    switch(triple_.getFileType()) {
        case Triple::FileType::ELF:
            switch(sect) {
                case SectionType::Text:
                    os << ".text";
                    break;
                case SectionType::BSS:
                    os << ".bss";
                    break;
                case SectionType::Data:
                    os << ".data";
                    break;
                case SectionType::Rodata:
                    os << ".rodata";
                    break;
            }
            break;
        case Triple::FileType::COFF:
            switch(sect) {
                case SectionType::Text:
                    os << ".text";
                    break;
                case SectionType::BSS:
                    os << ".bss";
                    break;
                case SectionType::Data:
                    os << ".data";
                    break;
                case SectionType::Rodata:
                    os << ".rdata";
                    break;
            }
            break;
        case Triple::FileType::MachO:
            switch(sect) {
                case SectionType::Text:
                    os << "__TEXT,__text";
                    break;
                case SectionType::BSS:
                    os << "__TEXT,__const";
                    break;
                case SectionType::Data:
                    os << "__DATA,__data";
                    break;
                case SectionType::Rodata:
                    os << "__DATA,__bss";
                    break;
            }
            break;
    }
    os << '\n';
}

void AsmPrinter::emitAlignment(raw_stream& os, Alignment alignment) const {
    if(alignment.getAlignment() <= 1) return;

    os << ".p2align " << alignment.getPow2() << '\n';
}

} // namespace inr