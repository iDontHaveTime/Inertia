// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/Target/x86/x86AsmPrinter.h>

namespace inr::x86 {
#ifdef INERTIA_INCLUDE_X86

static inline void emitReg(raw_stream& os, Register reg,
                           const RegisterInfo* regInfo) {
    os << '%';
    if(reg.isPhysical()) {
        os << regInfo->getName(reg);
    }
    else if(reg.isVirtual()) {
        os << "vreg" << reg.getIndex();
    }
}

static inline void emitMOP(raw_stream& os, const MachineOperand& mo,
                           const RegisterInfo* regInfo) {
    switch(mo.getKind()) {
        case MachineOperand::Kind::Reg:
            emitReg(os, mo.getReg(), regInfo);
            break;
        case MachineOperand::Kind::Imm:
            os << '$' << mo.getImm()->getValue();
            break;
        case MachineOperand::Kind::Global:
            break;
        case MachineOperand::Kind::DereferenceReg:
            MemOperand memop = mo.getMem();
            if(memop.getOffset()) {
                os << memop.getOffset();
            }
            os << '(';
            emitReg(os, memop.getRegister(), regInfo);
            os << ')';
            break;
    }
}

void x86AsmPrinter::emitMI(raw_stream& os, const MachineInst& mi) const {
    os << '\t';

    uint32_t asmidx = mi.getOp();
    const char* asmstr = getTriple().getTIRAsmStr();

    while(asmidx) {
        while(*asmstr) asmstr++;
        asmstr++;
        asmidx--;
    }

    os << asmstr << ' ';

    for(size_t i = 0; i < mi.getOperands().size(); i++) {
        if(i) os << ", ";
        emitMOP(os, mi.getOperand(i), getRegisterInfo());
    }

    os << '\n';
}

#else

void x86AsmPrinter::emitMI(raw_stream& os, const MachineInst& mi) const {}

#endif

} // namespace inr::x86