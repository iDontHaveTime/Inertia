// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/ISel/TargetTree.h>
#include <inr/MIR/MachineBlock.h>
#include <inr/MIR/MachineFunction.h>
#include <inr/MIR/MachineInst.h>
#include <inr/MIR/MachineModule.h>
#include <inr/MIR/MachineOperand.h>
#include <inr/Support/Stream.h>

namespace inr {

void printMachineOperandReg(raw_stream& os, const MachineOperand& mop) {
    Register reg = mop.getReg();
    switch(reg.getKind()) {
        case Register::Kind::Physical:
            os << "%reg";
            break;
        case Register::Kind::Virtual:
            os << "%vreg";
            break;
    }

    os << reg.getIndex();
}

void printMachineOperandImm(raw_stream& os, const MachineOperand& mop) {
    os << (int64_t)mop.getImm();
}

void printMachineOperandFrameIndex(raw_stream& os, const MachineOperand& mop) {
    os << "[frame " << mop.getFrameIndex() << ']';
}

void printMachineOperandBlockAddr(raw_stream&, const MachineOperand&) {}

void printMachineOperand(raw_stream& os, const MachineOperand& mop) {
    switch(mop.getKind()) {
        case MachineOperand::Kind::Reg:
            printMachineOperandReg(os, mop);
            break;
        case MachineOperand::Kind::Imm:
            printMachineOperandImm(os, mop);
            break;
        case MachineOperand::Kind::FrameIndex:
            printMachineOperandFrameIndex(os, mop);
            break;
        case MachineOperand::Kind::BlockAddr:
            printMachineOperandBlockAddr(os, mop);
            break;
    }
}

void printMachineInstruction(raw_stream& os, const TreeNodeBuilder& builder,
                             const MachineInst& i) {
    if(i.getOp() == COPY) {
        os << "\tCOPY ";
    }
    else {
        os << '\t' << Walker::getFromOpcode(builder, i.getOp())->getName()
           << ' ';
    }

    for(const MachineOperand& mop : i.getOperands()) {
        if(&mop != i.getOperands().data()) os << ", ";
        printMachineOperand(os, mop);
    }

    os << '\n';
}

void printMachineBlock(raw_stream& os, const TreeNodeBuilder& builder,
                       const MachineBlock& b) {
    os << b.getName() << ":\n";

    for(const MachineInst& i : b.getInstructions()) {
        printMachineInstruction(os, builder, i);
    }
}

void MachineModule::print(raw_stream& os,
                          const TreeNodeBuilder& builder) const {
    os << "; Machine Module, cannot be parsed back into source.\n";
    os << "; Machine Module = " << getName() << '\n';

    for(const MachineFunction& f : getFunctions()) {
        os << f.getName() << "(){\n";

        for(const MachineBlock& b : f.getBlocks()) {
            printMachineBlock(os, builder, b);
        }

        os << "}\n";
    }
}

} // namespace inr