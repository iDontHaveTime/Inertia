// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/IR/ArgDef.h>
#include <inr/IR/Type.h>
#include <inr/Support/Assert.h>
#include <inr/Support/Unreachable.h>
#include <inr/TIR/CCLowering.h>
#include <inr/TIR/TIRT.h>
#include <inr/TIR/TInst.h>
#include <inr/TIR/TOperand.h>
#include <inr/TIR/Translator.h>
#include <inr/Target/Register.h>

#include "x86Defs.h"

namespace inr::x86 {

/// @brief SystemV ABI for x86_64.
class x86_64_SysV_CC_Lowering : public TIRCCLowering {
    /// @brief 3.2.3 Parameter passing.
    enum ParameterPassing {
        INTEGER,
    };

    bool lowerReturn(TBlock* tblk, const RetInst& ret, const FuncDef&,
                     const Translator& om) const override {
        const TOperand* op = om.findOperand(ret.getRetVal());
        if(!op) {
            inr_assert(false,
                       "x86_64_SysV_CC_Lowering lowerReturn(): trying to "
                       "return a def that was not lowered");
            return false;
        }

        switch(op->getKind()) {
            case TOperand::Register:
                if(x86_registers.inFamily(A_FAMILY, op->getRegN())) return true;
            case TOperand::VRegister:
                break;
            case TOperand::Immediate:
            case TOperand::BImmediate:
                inr_assert(false,
                           "x86_64_SysV_CC_Lowering lowerReturn(): trying to "
                           "return an immediate");
                return false;
        }

        unsigned width = 0;

        switch(ret.getRetVal()->getType()->getID()) {
            case Type::Integer:
                width =
                    ((const IntType*)ret.getRetVal()->getType())->getWidth();
                break;
            case Type::Pointer:
                width = 64;
                break;
            case Type::Void:
            case Type::Block:
            case Type::Function:
                inr_assert(false,
                           "x86_64_SysV_CC_Lowering lowerReturn(): trying to "
                           "return an illegal type");
                return false;
            case Type::Float: {
                width = ((const FPType*)ret.getRetVal()->getType())->getWidth();
                if(width != 32 && width != 64) {
                    inr_assert(false,
                               "x86_64_SysV_CC_Lowering lowerReturn(): "
                               "unsupported fp format");
                    return false;
                }
                auto cpy =
                    tblk->addInst(TInst::gCopy, inr::TIRT::createBit(width));
                cpy->addOperand(TOperand::createReg(XMM0));
                cpy->addOperand(*op);
                return true;
            }
        }

        if(width == 0) {
            inr_assert(false,
                       "x86_64_SysV_CC_Lowering lowerReturn(): failed to "
                       "select width");
            return false;
        }
        else if(width != 8 && width != 16 && width != 32 && width != 64) {
            inr_assert(
                false,
                "x86_64_SysV_CC_Lowering lowerReturn(): illegal width given");
            return false;
        }

        inr::TIRT type = TIRT::createBit(width);

        auto reg = x86_registers.getRegisterWithTypeFromFamily(type, A_FAMILY);
        if(!reg) {
            inr_assert(false,
                       "x86_64_SysV_CC_Lowering lowerReturn(): failed to find "
                       "register to return");
            return false;
        }

        auto cpy = tblk->addInst(TInst::gCopy, type);
        cpy->addOperand(TOperand::createReg(reg->id));
        cpy->addOperand(*op);

        return true;
    }

    bool lowerArgs(TBlock* entry, const TSymbol* sym,
                   Translator& ts) const override {
        unsigned fpArgs = 0;
        unsigned iArgs = 0;
        for(unsigned i = 0; i < sym->getOriginal()->getNumArgs(); i++) {
            const ArgDef* ad = sym->getOriginal()->getArg(i);

            unsigned width = 0;
            switch(ad->getType()->getID()) {
                case Type::Integer:
                    width = ((const IntType*)ad->getType())->getWidth();
                    break;
                case Type::Pointer:
                    width = 64;
                    break;
                case Type::Void:
                case Type::Block:
                case Type::Function:
                    inr_assert(false,
                               "x86_64_SysV_CC_Lowering lowerArgs(): trying to "
                               "lower an illegal type");
                    return false;
                case Type::Float:
                    width = ((const FPType*)ad->getType())->getWidth();
                    break;
            }
            inr::TIRT type = TIRT::createBit(width);

            if(!ad->getType()->isFloat()) {
                if(iArgs >= 6) {
                    inr_assert(false,
                               "x86_64_SysV_CC_Lowering lowerArgs(): stack "
                               "args not implemented");
                    return false;
                }

                if(width == 0) {
                    inr_assert(false,
                               "x86_64_SysV_CC_Lowering lowerArgs(): failed to "
                               "select width");
                    return false;
                }
                else if(width != 8 && width != 16 && width != 32 &&
                        width != 64) {
                    inr_assert(false,
                               "x86_64_SysV_CC_Lowering lowerArgs(): illegal "
                               "width given");
                    return false;
                }

                auto inst = entry->addInst(TInst::gCopy, type);
                inst->addOperand(ts.getVreg(ad));

                const Register* reg = nullptr;
                switch(iArgs++) {
                    case 0:
                        reg = x86_registers.getRegisterWithTypeFromFamily(
                            type, DI_FAMILY);
                        break;
                    case 1:
                        reg = x86_registers.getRegisterWithTypeFromFamily(
                            type, SI_FAMILY);
                        break;
                    case 2:
                        reg = x86_registers.getRegisterWithTypeFromFamily(
                            type, D_FAMILY);
                        break;
                    case 3:
                        reg = x86_registers.getRegisterWithTypeFromFamily(
                            type, C_FAMILY);
                        break;
                    case 4:
                        reg = x86_registers.getRegisterWithTypeFromFamily(
                            type, R8_FAMILY);
                        break;
                    case 5:
                        reg = x86_registers.getRegisterWithTypeFromFamily(
                            type, R9_FAMILY);
                        break;
                    default:
                        inr_unreachable("Checked beforehand");
                }

                if(!reg) {
                    inr_assert(
                        false,
                        "x86_64_SysV_CC_Lowering lowerArgs(): failed to find "
                        "register to lower arg");
                    return false;
                }

                inst->addOperand(TOperand::createReg(reg->id));
            }
            else if(ad->getType()->isFloat()) {
                if(fpArgs >= 8) {
                    inr_assert(false,
                               "x86_64_SysV_CC_Lowering lowerArgs(): stack "
                               "args not implemented");
                    return false;
                }

                auto inst = entry->addInst(TInst::gCopy, type);
                inst->addOperand(ts.getVreg(ad));

                switch(fpArgs++) {
                    case 0:
                        inst->addOperand(TOperand::createReg(XMM0));
                        break;
                    case 1:
                        inst->addOperand(TOperand::createReg(XMM1));
                        break;
                    case 2:
                        inst->addOperand(TOperand::createReg(XMM2));
                        break;
                    case 3:
                        inst->addOperand(TOperand::createReg(XMM3));
                        break;
                    case 4:
                        inst->addOperand(TOperand::createReg(XMM4));
                        break;
                    case 5:
                        inst->addOperand(TOperand::createReg(XMM5));
                        break;
                    case 6:
                        inst->addOperand(TOperand::createReg(XMM6));
                        break;
                    case 7:
                        inst->addOperand(TOperand::createReg(XMM7));
                        break;
                    default:
                        inr_unreachable("Checked beforehand");
                }
            }
        }
        return true;
    }
} x86_64_cc_lowering;

const TIRCCLowering* getAMD64SysVCC() {
    return &x86_64_cc_lowering;
}

} // namespace inr::x86
