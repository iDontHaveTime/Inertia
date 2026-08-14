// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_TUNIT_H
#define INERTIA_IR_TUNIT_H

/// @file IR/TUnit.h
/// @brief Provides a class representing a translation unit.

#include <inr/ADT/IList.h>
#include <inr/IR/ArgDef.h>
#include <inr/IR/BlockDef.h>
#include <inr/IR/ConstDef.h>
#include <inr/IR/FuncDef.h>
#include <inr/IR/Linkage.h>
#include <inr/IR/Type.h>
#include <inr/IR/TypeMap.h>
#include <inr/IR/UnDef.h>

#include <memory>
#include <string_view>
#include <vector>

namespace inr {

/// @brief IR Translation unit.
/// @note This class' equivalent is LLVM's `Module` class.
///
/// A `TUnit` represents a translation unit, for example this C code:
/// ```c
/// int main(int argc, char** argv) {
///     return 0;
/// }
/// ```
/// Would be represented as:
/// ```llvm
/// def fn signext i32 @main(i32 signext %argc, ptr %argv) {
/// entry:
///     ret i32 0
/// }
/// ```
/// Basically meaning this class holds all the declarations and definitions for
/// one unit.
class TUnit {
    /// @brief Used for debugging.
    /// @note Usually goes into the `.file` directive.
    std::string_view name_;
    ilist<FuncDef> funcs_;
    std::vector<std::unique_ptr<Def>> defStorage_;

public:
    TUnit(std::string_view name) : name_(name) {}

    ~TUnit() {
        funcs_.deleteNodes();
    }

    std::string_view getName() const {
        return name_;
    }

    const ilist<FuncDef>& getFuncs() const {
        return funcs_;
    }

    ilist<FuncDef>& getFuncs() {
        return funcs_;
    }

    FuncDef* createFunction(const FuncType* type, std::string_view name,
                            Linkage linkage, TypeExt retExt);
    BlockDef* createBlock(TypeMap& tm, FuncDef* to, std::string_view name);

    ConstDef* createConst(const IntType* type, const bigint& val);
    ConstDef* createConst(const IntType* type, bigint&& val);
    UnDef* createUndef(const Type* t);
};

} // namespace inr

#endif // INERTIA_IR_TUNIT_H
