// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_BUILDER_H
#define INERTIA_IR_BUILDER_H

/// @file IR/Builder.h
/// @brief Provides helper classes to make IR construction easier.

#include <inr/IR/ArgDef.h>
#include <inr/IR/BlockDef.h>
#include <inr/IR/ConstDef.h>
#include <inr/IR/FuncDef.h>
#include <inr/IR/GlobalDef.h>
#include <inr/IR/InstDef.h>
#include <inr/IR/Linkage.h>
#include <inr/IR/TUnit.h>
#include <inr/IR/Type.h>
#include <inr/IR/TypeMap.h>
#include <inr/Math/BigInt.h>

#include <string_view>

namespace inr {

class BlockBuilder {
    BlockDef* block_;

    BlockBuilder(BlockDef* block) : block_(block) {}

    friend class FuncBuilder;
    friend class IRBuilder;

public:
    BlockBuilder(const BlockBuilder&) = delete;
    BlockBuilder& operator=(const BlockBuilder&) = delete;

    BlockBuilder(BlockBuilder&&) noexcept = default;

    BlockDef* getBlock() {
        return block_;
    }

    const BlockDef* getBlock() const {
        return block_;
    }

    TypeMap& getMap() {
        return getBlock()->getParent()->getParent()->getTypeMap();
    }

    /// @brief Creates a return void instruction.
    /// @return *this
    BlockBuilder& createRetVoid() {
        RetInst::createRetVoid(getMap(), getBlock());
        return *this;
    }

    /// @brief Creates a return instruction.
    /// @param retVal Return value, if nullptr then it's a ret void.
    /// @return *this
    BlockBuilder& createRet(Def* retVal) {
        RetInst::createRet(getMap(), getBlock(), retVal);
        return *this;
    }

    /// @brief Creates a jump instruction.
    /// @param dest The block to jump to.
    /// @return Pointer to the instruction.
    JmpInst* createJmp(BlockDef* dest) {
        return JmpInst::createJmp(getMap(), getBlock(), dest);
    }

    /// @brief Creates a conditional jump instruction.
    /// @param condition An i1 (boolean) condition.
    /// @param iftrue The block to jump to if the condition is true.
    /// @param iffalse The block to jump to if the condition is false.
    /// @return Pointer to the instruction.
    JmpInst* createJmpCond(Def* condition, BlockDef* iftrue,
                           BlockDef* iffalse) {
        return JmpInst::createJmpCond(getMap(), getBlock(), condition, iftrue,
                                      iffalse);
    }

    /// @brief Creates a new comparison instruction.
    /// @param condition The condition for the comparison to output 1 (true).
    /// @param lhs Left hand side of the comparison.
    /// @param rhs Right hand side of the comparison.
    /// @param name The name for the result, can be {}.
    /// @return Pointer to the instruction.
    CmpInst* createCmp(CmpInst::CmpCond condition, Def* lhs, Def* rhs,
                       std::string_view name) {
        return CmpInst::createCmp(getMap(), getBlock(), condition, lhs, rhs,
                                  name);
    }

    /// @brief Creates a new multiplication instruction.
    /// @param lhs Left hand side.
    /// @param rhs Right hand side.
    /// @param name The name for the result, can be {}.
    /// @return Pointer to the instruction.
    MulInst* createMul(Def* lhs, Def* rhs, std::string_view name) {
        return MulInst::createMul(getBlock(), lhs, rhs, name);
    }

    /// @brief Creates a new unsigned division instruction.
    /// @param lhs Left hand side.
    /// @param rhs Right hand side.
    /// @param name The name for the result, can be {}.
    /// @return Pointer to the instruction.
    UDivInst* createUDiv(Def* lhs, Def* rhs, std::string_view name) {
        return UDivInst::createUDiv(getBlock(), lhs, rhs, name);
    }

    /// @brief Creates a new signed division instruction.
    /// @param lhs Left hand side.
    /// @param rhs Right hand side.
    /// @param name The name for the result, can be {}.
    /// @return Pointer to the instruction.
    SDivInst* createSDiv(Def* lhs, Def* rhs, std::string_view name) {
        return SDivInst::createSDiv(getBlock(), lhs, rhs, name);
    }

    /// @brief Creates a new stack slot.
    /// @param type The type to allocate.
    /// @param count The number of elements to allocate (so the size would be
    /// sizeof(type) * count).
    /// @param name The name for the result, can be {}.
    /// @return Pointer to the instruction.
    AllocaInst* createAlloca(const Type* type, Def* count,
                             std::string_view name) {
        return AllocaInst::createAlloca(getMap(), getBlock(), type, count,
                                        name);
    }
};

class FuncBuilder {
    FuncDef* func_;

    FuncBuilder(FuncDef* func) : func_(func) {}

    friend class IRBuilder;

public:
    FuncBuilder(const FuncBuilder&) = delete;
    FuncBuilder& operator=(const FuncBuilder&) = delete;

    FuncBuilder(FuncBuilder&&) noexcept = default;

    FuncBuilder& setArgName(unsigned argNum, std::string_view name) {
        func_->getArg(argNum)->setName(name);
        return *this;
    }

    FuncBuilder& setArgExt(unsigned argNum, TypeExt ext) {
        func_->getArg(argNum)->setExt(ext);
        return *this;
    }

    FuncBuilder& setArgExtAndName(unsigned argNum, TypeExt ext,
                                  std::string_view name) {
        return setArgExt(argNum, ext).setArgName(argNum, name);
    }

    BlockBuilder createBlock(std::string_view name) {
        return {func_->createBlock(name)};
    }

    FuncDef* getFunc() {
        return func_;
    }

    const FuncDef* getFunc() const {
        return func_;
    }

    TypeMap& getMap() {
        return func_->getParent()->getTypeMap();
    }
};

class IRBuilder {
    TUnit& unit_;

public:
    IRBuilder(TUnit& unit) : unit_(unit) {}

    IRBuilder(const IRBuilder&) = delete;
    IRBuilder& operator=(const IRBuilder&) = delete;

    IRBuilder(IRBuilder&&) noexcept = default;

    TUnit& getUnit() {
        return unit_;
    }

    const TUnit& getUnit() const {
        return unit_;
    }

    TypeMap& getMap() {
        return unit_.getTypeMap();
    }

    ConstDef* getConst(inr::bigint val) {
        return unit_.createConst(std::move(val));
    }

    ConstDef* getZeroConst(const IntType* type) {
        return getConst(inr::bigint(type->getWidth()));
    }

    ConstDef* getZeroConst(unsigned width) {
        return getConst(inr::bigint(width));
    }

    FuncBuilder createFunction(const FuncType* type, std::string_view name,
                               TypeExt retExt,
                               Linkage linkage = Linkage::Global) {
        return {unit_.createFunction(type, name, linkage, retExt)};
    }
};

} // namespace inr

#endif // INERTIA_IR_BUILDER_H
