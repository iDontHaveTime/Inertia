// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TARGETSYMBOL_H
#define INERTIA_TIR_TARGETSYMBOL_H

/// @file TIR/TargetSymbol.h
/// @brief Represents a lowered function.

#include <inr/ADT/IList.h>
#include <inr/IR/FuncDef.h>
#include <inr/TIR/TargetBlock.h>

namespace inr {

class TargetSymbol {
    const FuncDef& original_;
    ilist<TargetBlock> blocks_;

    friend class TargetMap;

    TargetSymbol(const FuncDef& original) : original_(original) {}

public:
    const FuncDef& getOriginal() const {
        return original_;
    }
};

} // namespace inr

#endif // INERTIA_TIR_TARGETSYMBOL_H
