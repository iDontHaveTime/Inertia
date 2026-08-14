// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/IR/BlockDef.h>
#include <inr/IR/ConstDef.h>
#include <inr/IR/FuncDef.h>
#include <inr/IR/InstDef.h>
#include <inr/IR/Type.h>
#include <inr/IR/UnDef.h>
#include <inr/Support/Assert.h>
#include <inr/Support/Unreachable.h>
#include <inr/TIR/CCLowering.h>
#include <inr/TIR/TIRT.h>
#include <inr/TIR/TInst.h>
#include <inr/TIR/TModule.h>
#include <inr/TIR/TOperand.h>
#include <inr/TIR/Translator.h>

namespace inr {

TOperand Translator::getVreg(const Def* def) {
    auto [e, v] = operandMap_.try_emplace(def, TOperand::createVReg(vregN_));
    if(v) {
        vregN_++;
    }
    return *e;
}

const TOperand* Translator::findOperand(const Def* def) const {
    return operandMap_.find(def);
}

bool Translator::translateRet(TBlock* tblk, const RetInst& ret,
                              const FuncDef& fd) {
    if(ret.isRetVoid()) {
        tblk->addInst(TInst::gRet, TIRT::createOther());
    }
    else {
        auto cclow = tinfo_->getTIRLowering(fd.getCC());
        if(!cclow) {
            inr_assert(false,
                       "Translator translateRet(): no calling convention "
                       "exists for the target");
            return false;
        }
        if(!cclow->lowerReturn(tblk, ret, fd, *this)) {
            inr_assert(false,
                       "Translator translateRet(): failed to lower return");
            return false;
        }
        tblk->addInst(TInst::gRet, TIRT::createOther());
    }

    return true;
}

bool Translator::translateJmp(TBlock* tblk, const JmpInst& jmp) {
    // TODO: Jump to block
    if(jmp.isNonConditional()) {
        tblk->addInst(TInst::gJmp, TIRT::createOther());
    }
    return true;
}

bool Translator::translateUnreachable(TBlock*, const UnreachableInst&) {
    // Maybe do something here.
    return true;
}

bool Translator::translateCmp(TBlock* tblk, const CmpInst& cmp) {
    auto tinst = tblk->addInst(TInst::gCmp, getType(cmp.getType()));
    tinst->addOperand(TOperand::createImm(cmp.getCond()));

    auto lhs = getVreg(cmp.getLhs());
    auto rhs = getVreg(cmp.getRhs());
    auto dest = getVreg(&cmp);

    tinst->addOperand(dest);
    tinst->addOperand(lhs);
    tinst->addOperand(rhs);

    return true;
}

bool Translator::translatePhi(TBlock* tblk, const PhiInst& phi) {
    return true;
}

bool Translator::translateAdd(TBlock* tblk, const AddInst& add) {
    return translateBinaryInst(tblk, add, TInst::gAdd);
}

bool Translator::translateSub(TBlock* tblk, const SubInst& sub) {
    return translateBinaryInst(tblk, sub, TInst::gSub);
}

bool Translator::translateMul(TBlock* tblk, const MulInst& mul) {
    return translateBinaryInst(tblk, mul, TInst::gMul);
}

bool Translator::translateUDiv(TBlock* tblk, const UDivInst& udiv) {
    return translateBinaryInst(tblk, udiv, TInst::gUDiv);
}

bool Translator::translateSDiv(TBlock* tblk, const SDivInst& sdiv) {
    return translateBinaryInst(tblk, sdiv, TInst::gSDiv);
}

bool Translator::translateURem(TBlock* tblk, const URemInst& urem) {
    return translateBinaryInst(tblk, urem, TInst::gURem);
}

bool Translator::translateSRem(TBlock* tblk, const SRemInst& srem) {
    return translateBinaryInst(tblk, srem, TInst::gSRem);
}

bool Translator::translateShl(TBlock* tblk, const ShlInst& shl) {
    return translateBinaryInst(tblk, shl, TInst::gShl);
}

bool Translator::translateLShr(TBlock* tblk, const LShrInst& lshr) {
    return translateBinaryInst(tblk, lshr, TInst::gLShr);
}

bool Translator::translateAShr(TBlock* tblk, const AShrInst& ashr) {
    return translateBinaryInst(tblk, ashr, TInst::gAShr);
}

bool Translator::translateAnd(TBlock* tblk, const AndInst& andInst) {
    return translateBinaryInst(tblk, andInst, TInst::gAnd);
}

bool Translator::translateOr(TBlock* tblk, const OrInst& orInst) {
    return translateBinaryInst(tblk, orInst, TInst::gOr);
}

bool Translator::translateXor(TBlock* tblk, const XorInst& xorInst) {
    return translateBinaryInst(tblk, xorInst, TInst::gXor);
}

bool Translator::translateBinaryInst(TBlock* tblk, const BinaryInst& inst,
                                     TInst::InstType it) {
    auto lhs = getVreg(inst.getLhs());
    auto rhs = getVreg(inst.getRhs());

    auto dest = getVreg(&inst);

    auto tinst = tblk->addInst(it, getType(inst.getType()));

    tinst->addOperand(dest);
    tinst->addOperand(lhs);
    tinst->addOperand(rhs);

    return true;
}

bool Translator::translateLoad(TBlock* tblk, const LoadInst& load) {
    auto src = getVreg(load.getFrom());
    auto dest = getVreg(&load);

    auto tinst = tblk->addInst(TInst::gLoad, getType(load.getType()));

    tinst->addOperand(dest);
    tinst->addOperand(src);

    return true;
}

bool Translator::translateStore(TBlock* tblk, const StoreInst& store) {
    auto dest = getVreg(store.getTo());
    auto src = getVreg(store.getFrom());

    auto tinst = tblk->addInst(TInst::gStore, TIRT::createOther());

    tinst->addOperand(dest);
    tinst->addOperand(src);

    return true;
}

bool Translator::translateAlloca(TBlock* tblk, const AllocaInst& alloca) {
    return true;
}

bool Translator::translateConst(TBlock* tblk, const ConstDef& cDef) {
    auto [e, v] = operandMap_.try_emplace(&cDef, TOperand::createVReg(vregN_));
    if(v) {
        vregN_++;
        auto inst = tblk->addInst(TInst::gInteger, getType(cDef.getType()));
        inst->addOperand(*e);
        inst->addOperand(TOperand::createBImm(&cDef.getInteger()));
    }
    return true;
}

bool Translator::translateUndef(TBlock* tblk, const UnDef& uDef) {
    auto [e, v] = operandMap_.try_emplace(&uDef, TOperand::createVReg(vregN_));
    if(v) {
        vregN_++;
        auto inst = tblk->addInst(TInst::gUndef, getType(uDef.getType()));
        inst->addOperand(*e);
    }
    return true;
}

bool Translator::translateInst(TBlock* tblk, const InstDef& inst,
                               const FuncDef& fd) {
    for(const Def* d : inst.getUses()) {
        switch(d->getDefType()) {
            case Def::ArgDefType:
            case Def::BlockDefType:
            case Def::FuncDefType:
            case Def::InstDefType:
                break;
            case Def::ConstDefType:
                translateConst(tblk, *(const ConstDef*)d);
                break;
            case Def::UnDefDefType:
                translateUndef(tblk, *(const UnDef*)d);
                break;
        }
    }

    switch(inst.getInstType()) {
        case InstDef::Ret:
            return translateRet(tblk, (const RetInst&)inst, fd);
        case InstDef::Jmp:
            return translateJmp(tblk, (const JmpInst&)inst);
        case InstDef::Unreachable:
            return translateUnreachable(tblk, (const UnreachableInst&)inst);
        case InstDef::Cmp:
            return translateCmp(tblk, (const CmpInst&)inst);
        case InstDef::Phi:
            return translatePhi(tblk, (const PhiInst&)inst);
        case InstDef::Add:
            return translateAdd(tblk, (const AddInst&)inst);
        case InstDef::Sub:
            return translateSub(tblk, (const SubInst&)inst);
        case InstDef::Load:
            return translateLoad(tblk, (const LoadInst&)inst);
        case InstDef::Store:
            return translateStore(tblk, (const StoreInst&)inst);
        case InstDef::Alloca:
            return translateAlloca(tblk, (const AllocaInst&)inst);
        case InstDef::Mul:
            return translateMul(tblk, (const MulInst&)inst);
        case InstDef::UDiv:
            return translateUDiv(tblk, (const UDivInst&)inst);
        case InstDef::SDiv:
            return translateSDiv(tblk, (const SDivInst&)inst);
        case InstDef::URem:
            return translateURem(tblk, (const URemInst&)inst);
        case InstDef::SRem:
            return translateSRem(tblk, (const SRemInst&)inst);
        case InstDef::Shl:
            return translateShl(tblk, (const ShlInst&)inst);
        case InstDef::LShr:
            return translateLShr(tblk, (const LShrInst&)inst);
        case InstDef::AShr:
            return translateAShr(tblk, (const AShrInst&)inst);
        case InstDef::And:
            return translateAnd(tblk, (const AndInst&)inst);
        case InstDef::Or:
            return translateOr(tblk, (const OrInst&)inst);
        case InstDef::Xor:
            return translateXor(tblk, (const XorInst&)inst);
    }
}

TBlock* Translator::translateBlock(TSymbol* sym, const BlockDef& blk) {
    auto tblk = sym->addBlock(&blk);
    return tblk;
}

TSymbol* Translator::translateFunc(TModule& mod, const FuncDef& func) {
    auto sym = mod.addSymbol(&func);
    return sym;
}

bool Translator::translateArgs(TSymbol* sym, TBlock* to) {
    auto cclow = tinfo_->getTIRLowering(sym->getOriginal()->getCC());
    if(!cclow) {
        inr_assert(false,
                   "Translator translateArgs(): no calling convention "
                   "exists for the target");
        return false;
    }
    if(!cclow->lowerArgs(to, sym, *this)) {
        inr_assert(false, "Translator translateArgs(): failed to lower args");
        return false;
    }

    return true;
}

TModule Translator::translate(const TUnit& unit) {
    TModule mod(&unit);

    for(const FuncDef& func : unit.getFuncs()) {
        auto sym = translateFunc(mod, func);

        for(const BlockDef& blk : func.getBlocks()) {
            auto tblk = translateBlock(sym, blk);
            if(&blk == &func.getBlocks().front()) {
                translateArgs(sym, tblk);
            }

            for(const InstDef& inst : blk.getInstructions()) {
                translateInst(tblk, inst, func);
            }
        }
    }

    return mod;
}

TIRT Translator::getType(const Type* t) const {
    return getType(t, tinfo_);
}

TIRT Translator::getType(const Type* t, const TargetInfo* tinfo) {
    switch(t->getID()) {
        case Type::Integer:
            return TIRT::createBit(((const IntType*)t)->getWidth());
        case Type::Pointer:
            return TIRT::createBit(tinfo->getPtrWidth());
        case Type::Float:
            return TIRT::createBit(((const FPType*)t)->getWidth());
        case Type::Void:
        case Type::Block:
        case Type::Function:
            return TIRT::createOther();
        default:
            inr_unreachable("Type should only be one of these");
    }
}

} // namespace inr
