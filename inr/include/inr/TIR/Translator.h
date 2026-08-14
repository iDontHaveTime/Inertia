// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TRANSLATOR
#define INERTIA_TIR_TRANSLATOR

/// @file TIR/Translator.h
/// @brief Translates IR to TIR.

#include <inr/ADT/HMap.h>
#include <inr/IR/BlockDef.h>
#include <inr/IR/ConstDef.h>
#include <inr/IR/Def.h>
#include <inr/IR/FuncDef.h>
#include <inr/IR/InstDef.h>
#include <inr/IR/TUnit.h>
#include <inr/IR/Type.h>
#include <inr/IR/UnDef.h>
#include <inr/TIR/TBlock.h>
#include <inr/TIR/TIRT.h>
#include <inr/TIR/TInst.h>
#include <inr/TIR/TModule.h>
#include <inr/TIR/TOperand.h>
#include <inr/TIR/TSymbol.h>
#include <inr/Target/TargetDesc.h>

#include <cstdint>

namespace inr {

class Translator {
public:
    using OperandMap = HMap<const Def*, TOperand>;

private:
    const TargetInfo* tinfo_;
    OperandMap operandMap_;
    uint32_t vregN_ = 0;

public:
    Translator(const TargetInfo* tinfo) : tinfo_(tinfo) {}

    TOperand getVreg(const Def* def);
    const TOperand* findOperand(const Def* def) const;

    bool translateRet(TBlock* tblk, const RetInst& ret, const FuncDef& fd);
    bool translateJmp(TBlock* tblk, const JmpInst& jmp);
    bool translateUnreachable(TBlock* tblk, const UnreachableInst& unreachable);
    bool translateCmp(TBlock* tblk, const CmpInst& cmp);
    bool translatePhi(TBlock* tblk, const PhiInst& phi);
    bool translateAdd(TBlock* tblk, const AddInst& add);
    bool translateSub(TBlock* tblk, const SubInst& sub);
    bool translateBinaryInst(TBlock* tblk, const BinaryInst& inst,
                             TInst::InstType it);
    bool translateLoad(TBlock* tblk, const LoadInst& load);
    bool translateStore(TBlock* tblk, const StoreInst& store);
    bool translateAlloca(TBlock* tblk, const AllocaInst& alloca);
    bool translateMul(TBlock* tblk, const MulInst& mul);
    bool translateUDiv(TBlock* tblk, const UDivInst& udiv);
    bool translateSDiv(TBlock* tblk, const SDivInst& sdiv);
    bool translateURem(TBlock* tblk, const URemInst& urem);
    bool translateSRem(TBlock* tblk, const SRemInst& srem);
    bool translateShl(TBlock* tblk, const ShlInst& shl);
    bool translateLShr(TBlock* tblk, const LShrInst& lshr);
    bool translateAShr(TBlock* tblk, const AShrInst& ashr);
    bool translateAnd(TBlock* tblk, const AndInst& andInst);
    bool translateOr(TBlock* tblk, const OrInst& orInst);
    bool translateXor(TBlock* tblk, const XorInst& xorInst);

    bool translateConst(TBlock*, const ConstDef& cDef);
    bool translateUndef(TBlock*, const UnDef& uDef);

    bool translateInst(TBlock* tblk, const InstDef& inst, const FuncDef& fd);

    bool translateArgs(TSymbol* sym, TBlock* to);

    TBlock* translateBlock(TSymbol* sym, const BlockDef& blk);

    TSymbol* translateFunc(TModule& mod, const FuncDef& func);

    TModule translate(const TUnit& unit);

    TIRT getType(const Type* t) const;
    static TIRT getType(const Type* t, const TargetInfo* tinfo);
};

} // namespace inr

#endif // INERTIA_TIR_TRANSLATOR
