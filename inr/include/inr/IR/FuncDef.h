// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_FUNCDEF_H
#define INERTIA_IR_FUNCDEF_H

/// @file IR/FuncDef.h
/// @brief Represents a function def.

#include <inr/ADT/IList.h>
#include <inr/IR/ArgDef.h>
#include <inr/IR/BlockDef.h>
#include <inr/IR/CallingConv.h>
#include <inr/IR/Def.h>
#include <inr/IR/GlobalDef.h>
#include <inr/IR/Linkage.h>
#include <inr/IR/Type.h>

namespace inr {

class TUnit;

/// @brief Function definition.
class FuncDef : public GlobalDef, public ilist_node<FuncDef> {
    std::vector<ArgDef> args_;
    ilist<BlockDef> blocks_;
    CallingConv cc_ = CallingConv::Default;
    TypeExt ext_;

    void initArgs() {
        const FuncType* type = getElemType()->as<FuncType>();
        for(unsigned i = 0; i < type->getNumArgs(); i++) {
            args_.emplace_back(type->getArg(i), std::string_view{}, i,
                               TypeExt::NoExt);
        }
    }

    FuncDef(const PtrType* ptr, const FuncType* type, std::string_view name,
            Linkage linkage, TypeExt retExt) :
        GlobalDef(type, linkage, ptr, FuncDefType, name), ext_(retExt) {
        initArgs();
    }

    BlockDef* createBlock(const BlockType* bt, std::string_view name);

    friend class TUnit;

public:
    ~FuncDef() override {
        blocks_.deleteNodes();
    }

    ilist<BlockDef>& getBlocks() {
        return blocks_;
    }

    const ilist<BlockDef>& getBlocks() const {
        return blocks_;
    }

    unsigned getNumArgs() const {
        return args_.size();
    }

    ArgDef* getArg(unsigned i) {
        return &args_[i];
    }

    const ArgDef* getArg(unsigned i) const {
        return &args_[i];
    }

    TypeExt getRetExt() const {
        return ext_;
    }

    CallingConv getCC() const {
        return cc_;
    }

    void setCC(CallingConv cc) {
        cc_ = cc;
    }
};

} // namespace inr

#endif // INERTIA_IR_FUNCDEF_H
