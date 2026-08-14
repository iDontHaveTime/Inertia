// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_ARGDEF_H
#define INERTIA_IR_ARGDEF_H

/// @file IR/ArgDef.h
/// @brief Def for a function argument.

#include <inr/IR/Def.h>

namespace inr {

/// @brief How should the type be extended when passed in or returned.
enum class TypeExt : unsigned char {
    NoExt,
    SignExt,
    ZeroExt,
};

/// @brief Represents a function argument.
class ArgDef : public Def {
    unsigned num_;
    TypeExt ext_;

public:
    ArgDef(const Type* type, std::string_view name, unsigned num, TypeExt ext) :
        Def(type, ArgDefType, name), num_(num), ext_(ext) {}

    /// @brief Returns the position of this argument in a function.
    unsigned getArgPos() const {
        return num_;
    }

    void setExt(TypeExt ext) {
        ext_ = ext;
    }

    TypeExt getExt() const {
        return ext_;
    }
};

} // namespace inr

#endif // INERTIA_IR_ARGDEF_H
