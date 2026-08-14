// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TMODULE_H
#define INERTIA_TIR_TMODULE_H

/// @file TIR/TModule.h
/// @brief Provides the target module class, lowered from the TUnit class.

#include <inr/ADT/IList.h>
#include <inr/IR/FuncDef.h>
#include <inr/IR/TUnit.h>
#include <inr/TIR/TSymbol.h>

namespace inr {

class TModule {
    const TUnit* original_;
    ilist<TSymbol> symbols_;

public:
    TModule(const TUnit* original) : original_(original) {}

    TModule(const TModule&) = delete;
    TModule& operator=(const TModule&) = delete;

    TModule(TModule&&) noexcept = default;
    TModule& operator=(TModule&&) noexcept = default;

    ~TModule() {
        symbols_.deleteNodes();
    }

    TSymbol* addSymbol(const FuncDef* from) {
        return symbols_.push_back(new TSymbol(from));
    }

    const TUnit* getOriginal() const {
        return original_;
    }

    const ilist<TSymbol>& getSyms() const {
        return symbols_;
    }
};

} // namespace inr

#endif // INERTIA_TIR_TMODULE_H
