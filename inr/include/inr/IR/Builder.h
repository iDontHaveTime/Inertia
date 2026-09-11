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
    TypeMap& map_;

    BlockBuilder(BlockDef* block, TypeMap& map) : block_(block), map_(map) {}

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
        return map_;
    }

    /// @brief Creates a return void instruction.
    BlockBuilder& createRetVoid() {
        RetInst::createRetVoid(map_, block_);
        return *this;
    }

    /// @brief Creates a return instruction.
    /// @param retVal Return value, if nullptr then it's a ret void.
    BlockBuilder& createRet(Def* retVal) {
        RetInst::createRet(map_, block_, retVal);
        return *this;
    }
};

class FuncBuilder {
    FuncDef* func_;
    TypeMap& map_;

    FuncBuilder(FuncDef* func, TypeMap& map) : func_(func), map_(map) {}

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
        return {func_->createBlock(map_, name), map_};
    }

    FuncDef* getFunc() {
        return func_;
    }

    const FuncDef* getFunc() const {
        return func_;
    }

    TypeMap& getMap() {
        return map_;
    }
};

class IRBuilder {
    TUnit& unit_;
    TypeMap& map_;

public:
    IRBuilder(TUnit& unit, TypeMap& map) : unit_(unit), map_(map) {}

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
        return map_;
    }

    ConstDef* getConst(const IntType* type, inr::bigint val) {
        return unit_.createConst(type, std::move(val));
    }

    ConstDef* getConst(unsigned width, inr::bigint val) {
        return getConst(map_.getInt(width), std::move(val));
    }

    ConstDef* getZeroConst(const IntType* type) {
        return getConst(type, inr::bigint(type->getWidth()));
    }

    ConstDef* getZeroConst(unsigned width) {
        return getConst(width, inr::bigint(width));
    }

    FuncBuilder createFunction(const FuncType* type, std::string_view name,
                               TypeExt retExt,
                               Linkage linkage = Linkage::Global) {
        FuncDef* func = unit_.createFunction(map_, type, name, linkage, retExt);
        return {func, map_};
    }
};

} // namespace inr

#endif // INERTIA_IR_BUILDER_H
