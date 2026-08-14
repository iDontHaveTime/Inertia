// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/IR/BlockDef.h>
#include <inr/IR/InstDef.h>

namespace inr {

RetInst* RetInst::createRet(TypeMap& tm, BlockDef* blk, Def* retVal) {
    if(retVal) {
        return (RetInst*)blk->getInstructions().push_back(
            new RetInst(retVal->getType(), retVal));
    }
    return (RetInst*)blk->getInstructions().push_back(
        new RetInst(tm.getVoid()));
}

RetInst* RetInst::createRetVoid(TypeMap& tm, BlockDef* blk) {
    return createRet(tm, blk, nullptr);
}

JmpInst* JmpInst::createJmp(TypeMap& tm, BlockDef* blk, Def* lbl) {
    return (JmpInst*)blk->getInstructions().push_back(
        new JmpInst(tm.getVoid(), lbl));
}

JmpInst* JmpInst::createJmpCond(TypeMap& tm, BlockDef* blk, Def* cond,
                                Def* iftrue, Def* iffalse) {
    return (JmpInst*)blk->getInstructions().push_back(
        new JmpInst(tm.getVoid(), cond, iftrue, iffalse));
}

CmpInst* CmpInst::createCmp(TypeMap& tm, BlockDef* blk, CmpCond cond, Def* lhs,
                            Def* rhs, std::string_view name) {
    return (CmpInst*)blk->getInstructions().push_back(
        new CmpInst(tm.getI1(), name, cond, lhs, rhs));
}

PhiInst* PhiInst::createPhi(BlockDef* blk, const Type* type,
                            std::string_view name) {
    return (PhiInst*)blk->getInstructions().push_back(new PhiInst(type, name));
}

AddInst* AddInst::createAdd(BlockDef* blk, Def* lhs, Def* rhs,
                            std::string_view name) {
    return (AddInst*)blk->getInstructions().push_back(
        new AddInst(lhs->getType(), name, lhs, rhs));
}

MulInst* MulInst::createMul(BlockDef* blk, Def* lhs, Def* rhs,
                            std::string_view name) {
    return (MulInst*)blk->getInstructions().push_back(
        new MulInst(lhs->getType(), name, lhs, rhs));
}

UDivInst* UDivInst::createUDiv(BlockDef* blk, Def* lhs, Def* rhs,
                               std::string_view name) {
    return (UDivInst*)blk->getInstructions().push_back(
        new UDivInst(lhs->getType(), name, lhs, rhs));
}

SDivInst* SDivInst::createSDiv(BlockDef* blk, Def* lhs, Def* rhs,
                               std::string_view name) {
    return (SDivInst*)blk->getInstructions().push_back(
        new SDivInst(lhs->getType(), name, lhs, rhs));
}

URemInst* URemInst::createURem(BlockDef* blk, Def* lhs, Def* rhs,
                               std::string_view name) {
    return (URemInst*)blk->getInstructions().push_back(
        new URemInst(lhs->getType(), name, lhs, rhs));
}

SRemInst* SRemInst::createSRem(BlockDef* blk, Def* lhs, Def* rhs,
                               std::string_view name) {
    return (SRemInst*)blk->getInstructions().push_back(
        new SRemInst(lhs->getType(), name, lhs, rhs));
}

SubInst* SubInst::createSub(BlockDef* blk, Def* lhs, Def* rhs,
                            std::string_view name) {
    return (SubInst*)blk->getInstructions().push_back(
        new SubInst(lhs->getType(), name, lhs, rhs));
}

ShlInst* ShlInst::createShl(BlockDef* blk, Def* lhs, Def* rhs,
                            std::string_view name) {
    return (ShlInst*)blk->getInstructions().push_back(
        new ShlInst(lhs->getType(), name, lhs, rhs));
}

LShrInst* LShrInst::createLShr(BlockDef* blk, Def* lhs, Def* rhs,
                               std::string_view name) {
    return (LShrInst*)blk->getInstructions().push_back(
        new LShrInst(lhs->getType(), name, lhs, rhs));
}

AShrInst* AShrInst::createAShr(BlockDef* blk, Def* lhs, Def* rhs,
                               std::string_view name) {
    return (AShrInst*)blk->getInstructions().push_back(
        new AShrInst(lhs->getType(), name, lhs, rhs));
}

AndInst* AndInst::createAnd(BlockDef* blk, Def* lhs, Def* rhs,
                            std::string_view name) {
    return (AndInst*)blk->getInstructions().push_back(
        new AndInst(lhs->getType(), name, lhs, rhs));
}

OrInst* OrInst::createOr(BlockDef* blk, Def* lhs, Def* rhs,
                         std::string_view name) {
    return (OrInst*)blk->getInstructions().push_back(
        new OrInst(lhs->getType(), name, lhs, rhs));
}

XorInst* XorInst::createXor(BlockDef* blk, Def* lhs, Def* rhs,
                            std::string_view name) {
    return (XorInst*)blk->getInstructions().push_back(
        new XorInst(lhs->getType(), name, lhs, rhs));
}

UnreachableInst* UnreachableInst::createUnreachable(TypeMap& tm,
                                                    BlockDef* blk) {
    return (UnreachableInst*)blk->getInstructions().push_back(
        new UnreachableInst(tm.getVoid()));
}

LoadInst* LoadInst::createLoad(BlockDef* blk, const Type* type, Def* from,
                               std::string_view name) {
    return (LoadInst*)blk->getInstructions().push_back(
        new LoadInst(type, name, from));
}

StoreInst* StoreInst::createStore(TypeMap& tm, BlockDef* blk, Def* to,
                                  Def* from) {
    return (StoreInst*)blk->getInstructions().push_back(
        new StoreInst(tm.getVoid(), to, from));
}

AllocaInst* AllocaInst::createAlloca(TypeMap& tm, BlockDef* blk,
                                     const Type* toAllocate, Def* count,
                                     std::string_view name) {
    return (AllocaInst*)blk->getInstructions().push_back(
        new AllocaInst(tm.getPtr(), name, toAllocate, count));
}

} // namespace inr
