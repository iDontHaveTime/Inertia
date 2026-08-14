// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TBLOCK
#define INERTIA_TIR_TBLOCK

/// @file TIR/TBlock.h
/// @brief Provides the target block class, lowers from BlockDef.

#include <inr/ADT/IList.h>
#include <inr/IR/BlockDef.h>
#include <inr/TIR/TIRT.h>
#include <inr/TIR/TInst.h>

namespace inr {

class TBlock : public ilist_node<TBlock> {
    const BlockDef* original_;
    ilist<TInst> insts_;

public:
    TBlock(const BlockDef* original) : original_(original) {}

    TBlock(const TBlock&) = delete;
    TBlock& operator=(const TBlock&) = delete;

    TBlock(TBlock&&) noexcept = default;
    TBlock& operator=(TBlock&&) noexcept = default;

    const ilist<TInst>& getInstructions() const {
        return insts_;
    }

    TInst* addInst(TInst::InstType instType, TIRT type) {
        return insts_.push_back(new TInst(instType, type));
    }

    const BlockDef* getOriginal() const {
        return original_;
    }

    ~TBlock() {
        insts_.deleteNodes();
    }
};

} // namespace inr

#endif // INERTIA_TIR_TBLOCK
