// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TARGETMAP_H
#define INERTIA_TIR_TARGETMAP_H

/// @file TIR/TargetMap.h
/// @brief Provides a target map class for a lowered IR unit.

#include <inr/ADT/HMap.h>
#include <inr/IR/FuncDef.h>
#include <inr/TIR/TargetSymbol.h>

namespace inr {

/// @brief Map of lowered functions.
class TargetMap {
    HMap<const FuncDef*, std::unique_ptr<TargetSymbol>> funcMap_;

public:
    TargetSymbol* getSymbol(const FuncDef* func) {
        if(auto v = funcMap_.find(func)) return v->get();
        auto [v, _] = funcMap_.try_emplace(func, new TargetSymbol(*func));
        return v->get();
    }
};

} // namespace inr

#endif // INERTIA_TIR_TARGETMAP_H
