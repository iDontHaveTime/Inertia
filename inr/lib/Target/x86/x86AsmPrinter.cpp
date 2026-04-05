// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/DAG/DAGNode.h>
#include <inr/Target/ISel.h>
#include <inr/Target/x86/x86AsmPrinter.h>
#define x86_NEED_STRTABLE
#define x86_NEED_OPCODES
#include <inr/Target/x86/x86Instructions.inc>

namespace inr::x86 {

static inline void emitMOP(raw_stream& os, const MachineOperand& mo,
                           const RegisterInfo* regInfo) {
    switch(mo.getKind()) {
        case MachineOperand::Kind::Reg:
            os << '%';
            if(mo.getReg().isPhysical()) {
                os << regInfo->getName(mo.getReg());
            }
            else if(mo.getReg().isVirtual()) {
                os << "vreg" << mo.getReg().getIndex();
            }
            break;
        case MachineOperand::Kind::Imm:
            os << '$' << mo.getImm()->getValue();
            break;
        case MachineOperand::Kind::FrameIndex:
            break;
        case MachineOperand::Kind::BlockAddr:
            break;
    }
}

void x86AsmPrinter::emitMI(raw_stream& os, const MachineInst& mi) const {
    os << '\t';
    if(!ISel::isTargetInst(mi.getOp())) {
        os << "error inst\n";
        return;
    }

    const char* asmstr =
        OpcodeAsmStr[(mi.getOp() - (uint32_t)Opcodes::OPCODE_START) - 1];

    os << asmstr << ' ';

    size_t idx = 0;
    for(auto it = mi.getOperands().rbegin(); it != mi.getOperands().rend();
        ++it) {
        if(idx++) {
            os << ", ";
        }
        emitMOP(os, *it, getRegisterInfo());
    }

    os << '\n';
}

} // namespace inr::x86