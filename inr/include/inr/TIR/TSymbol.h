// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TSYMBOL_H
#define INERTIA_TIR_TSYMBOL_H

/// @file TIR/TSymbol.h
/// @brief Provides the target symbol class, lowered from a FuncDef.h

#include <inr/ADT/IList.h>
#include <inr/IR/BlockDef.h>
#include <inr/IR/FuncDef.h>
#include <inr/TIR/TBlock.h>

namespace inr {

class TSymbol : public ilist_node<TSymbol> {
    const FuncDef* original_;
    ilist<TBlock> blocks_;

public:
    TSymbol(const FuncDef* original) : original_(original) {}

    TSymbol(const TSymbol&) = delete;
    TSymbol& operator=(const TSymbol&) = delete;

    TSymbol(TSymbol&&) noexcept = default;
    TSymbol& operator=(TSymbol&&) noexcept = default;

    const FuncDef* getOriginal() const {
        return original_;
    }

    TBlock* addBlock(const BlockDef* original) {
        return blocks_.push_back(new TBlock(original));
    }

    const ilist<TBlock>& getBlocks() const {
        return blocks_;
    }

    ~TSymbol() {
        blocks_.deleteNodes();
    }
};

} // namespace inr

#endif // INERTIA_TIR_TSYMBOL_H
