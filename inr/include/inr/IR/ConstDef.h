// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_CONSTDEF_H
#define INERTIA_IR_CONSTDEF_H

/// @file IR/ConstDef.h
/// @brief Defines an integer constant.

#include <inr/IR/Def.h>
#include <inr/IR/Type.h>
#include <inr/Math/BigInt.h>
#include <inr/Support/Assert.h>

namespace inr {

/// @brief Represents an integer constant.
class ConstDef : public Def {
    bigint int_;

    ConstDef(const IntType* type, const bigint& bi) :
        Def(type, ConstDefType, {}), int_(bi) {
        inr_assert(type->getWidth() == int_.getBits(),
                   "ConstDef ConstDef(): bits do not match");
    }

    ConstDef(const IntType* type, bigint&& bi) :
        Def(type, ConstDefType, {}), int_(std::move(bi)) {
        inr_assert(type->getWidth() == int_.getBits(),
                   "ConstDef ConstDef(): bits do not match");
    }

    friend class TUnit;

public:
    const bigint& getInteger() const {
        return int_;
    }

    bigint& getInteger() {
        return int_;
    }
};

} // namespace inr

#endif // INERTIA_IR_CONSTDEF_H
