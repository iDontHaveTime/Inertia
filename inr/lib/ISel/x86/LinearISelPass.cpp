// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/IR/Argument.h>
#include <inr/IR/Constant.h>
#include <inr/IR/Context.h>
#include <inr/IR/Function.h>
#include <inr/IR/Instruction.h>
#include <inr/IR/Type.h>
#include <inr/IR/Value.h>
#include <inr/ISel/TargetTree.h>
#include <inr/ISel/x86/LinearISelPass.h>
#include <inr/ISel/x86/TargetTree.h>
#include <inr/MIR/MachineBlock.h>
#include <inr/MIR/MachineFunction.h>
#include <inr/MIR/MachineInst.h>
#include <inr/MIR/MachineOperand.h>
#include <inr/MIR/Register.h>
#include <inr/Support/Compiler.h>

#include <stdexcept>

#include "inr/ADT/ArrView.h"

/// @file lib/ISel/x86/LinearISelPass.cpp
/// @brief This file implements the linear instruction selector pass for x86.
///
/// This is the main way to do instruction selection on x86.

namespace inr::x86 {

#if __has_attribute(used)
#define USED [[gnu::used]]
#else
#define USED
#endif

USED constexpr Register RAX = {0, Register::Kind::Physical};
USED constexpr Register RCX = {1, Register::Kind::Physical};
USED constexpr Register RDX = {2, Register::Kind::Physical};
USED constexpr Register RBX = {3, Register::Kind::Physical};
USED constexpr Register RSP = {4, Register::Kind::Physical};
USED constexpr Register RBP = {5, Register::Kind::Physical};
USED constexpr Register RSI = {6, Register::Kind::Physical};
USED constexpr Register RDI = {7, Register::Kind::Physical};

USED constexpr Register R8 = {8, Register::Kind::Physical};
USED constexpr Register R9 = {9, Register::Kind::Physical};
USED constexpr Register R10 = {10, Register::Kind::Physical};
USED constexpr Register R11 = {11, Register::Kind::Physical};
USED constexpr Register R12 = {12, Register::Kind::Physical};
USED constexpr Register R13 = {13, Register::Kind::Physical};
USED constexpr Register R14 = {14, Register::Kind::Physical};
USED constexpr Register R15 = {15, Register::Kind::Physical};

#undef USED

struct ReturnInfo {
    Register reg;
};

ReturnInfo getReturnInfo(const Type*) {
    return {RAX};
}

arrview<Register> getArgs() {
    static Register ArgsSysV[] = {RDI, RSI, RDX, RCX, R8, R9};

    return ArgsSysV;
}

void lowerReturn(const InrContext& ctx, MachineFunction& func, Instruction& ins,
                 MachineBlock* mblock) {
    if(ins.getType() != ctx.getVoid()) {
        if(ins.getType()->getTypeID() == Type::TypeID::Integer) {
            ReturnInfo rinfo = getReturnInfo(ins.getType());

            MachineInst::create(
                COPY, mblock,
                {MachineOperand::createReg(rinfo.reg),
                 MachineOperand::createReg(
                     Register{func.getVregFromValue(ins.getOperand(0)).value(),
                              Register::Kind::Virtual})});
        }
    }

    MachineInst::create(OpcodeType(Opcodes::RET), mblock);
}

Operands getOpFromValue(const Value* val) {
    switch(val->getValueID()) {
        case Value::ValueID::Argument:
            [[fallthrough]];
        case Value::ValueID::Instruction:
            break;
        case Value::ValueID::ConstantInt:
            return Operands::Imm;
            break;
        default:
            throw std::runtime_error("Wrong value provided in instruction");
    }
    return Operands::Reg;
}

void lowerAdd(const InrContext&, MachineFunction& mfunc, Instruction& ins,
              MachineBlock* mblock, const TargetTree* tt) {
    const Value* lhs = ins.getOperand(0);
    const Value* rhs = ins.getOperand(1);

    Operands opLhs = getOpFromValue(lhs);
    Operands opRhs = getOpFromValue(rhs);

    OperandDesc descLhs(OperandType(opLhs), lhs->getType());
    OperandDesc descRhs(OperandType(opRhs), rhs->getType());

    OperandSignature sig({descLhs, descRhs});

    ivec<MachineOperand, 3> mops;

    mops.emplace_back(MachineOperand::createReg(
        Register(mfunc.newVregValue(&ins), Register::Kind::Virtual)));

    if(opLhs == Operands::Reg) {
        mops.emplace_back(MachineOperand::createReg(Register(
            mfunc.getVregFromValue(lhs).value(), Register::Kind::Virtual)));
    }
    else {
        mops.emplace_back(
            MachineOperand::createImm(((const ConstantInt*)lhs)->getValue()));
    }

    if(opRhs == Operands::Reg) {
        mops.emplace_back(MachineOperand::createReg(Register(
            mfunc.getVregFromValue(rhs).value(), Register::Kind::Virtual)));
    }
    else {
        mops.emplace_back(
            MachineOperand::createImm(((const ConstantInt*)rhs)->getValue()));
    }

    MachineInst::create(
        Walker::walk(tt, InstructionType(InstructionTypes::ADD), sig)->getOp(),
        mblock, std::move(mops));
}

void lowerInstruction(const InrContext& ctx, MachineFunction& func,
                      Instruction& ins, MachineBlock* mblock,
                      const TargetTree* tt) {
    switch(ins.getID()) {
        case Instruction::InstructionID::RETURN:
            lowerReturn(ctx, func, ins, mblock);
            break;
        case Instruction::InstructionID::ADD:
            lowerAdd(ctx, func, ins, mblock, tt);
            break;
    }
}

void lowerBlock(const InrContext& ctx, Function& func, MachineFunction& mfunc,
                Block& b, const TargetTree* tt) {
    MachineBlock* block = mfunc.newBlock(b.getName());

    if(block == &*mfunc.getBlocks().begin()) {
        arrview<Register> liveins = getArgs();

        for(size_t i = 0; i < func.getArgs().size(); i++) {
            ivec<MachineOperand, 3> mops;
            if(i < liveins.size()) {
                mops.emplace_back(MachineOperand::createReg(
                    Register(mfunc.newVregValue(&func.getArgs()[i]),
                             Register::Kind::Virtual)));
                mops.emplace_back(MachineOperand::createReg(liveins[i]));
            }

            MachineInst::create(COPY, block, std::move(mops));
        }
    }

    for(Instruction& ins : b.getInstructions()) {
        lowerInstruction(ctx, mfunc, ins, block, tt);
    }
}

void lowerFunction(const InrContext& ctx, Function& func,
                   MachineFunction& mfunc, const TargetTree* tt) {
    for(Block& b : func.getBlocks()) {
        lowerBlock(ctx, func, mfunc, b, tt);
    }
}

void LinearISelPass::run(const InrContext& ctx, Module& mod,
                         MachineModule& mmod, const TargetTree* tt) {
    /// x86 Linear ISel Pass
    for(Function& f : mod.getFunctions()) {
        lowerFunction(ctx, f, *mmod.newFunction(f.getName()), tt);
    }
}

} // namespace inr::x86