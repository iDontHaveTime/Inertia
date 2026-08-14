// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_UNDEF_H
#define INERTIA_IR_UNDEF_H

/// @file IR/UnDef.h
/// @brief An undefined definition.

#include <inr/IR/Def.h>

namespace inr {

/// @brief Represents an undefined definition.
///
/// For example could be used in phi nodes:
/// ```llvm
/// %x = phi i32 [undef, %block1], [%y, %block2]
/// ```
class UnDef : public Def {
public:
    UnDef(const Type* type) : Def(type, UnDefDefType, {}) {}
};

} // namespace inr

#endif // INERTIA_IR_UNDEF_H
