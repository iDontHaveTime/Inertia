// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TIRBLOCK_H
#define INERTIA_TIR_TIRBLOCK_H

/// @file TIR/TIRBlock.h
/// @brief Contains the Target IR block class.

#include <inr/ADT/IList.h>
#include <inr/IR/Block.h>
#include <inr/TIR/TIRInstruction.h>

namespace inr {

/// @brief Represents a TIR block.
class TIRBlock : public ilist_node<TIRBlock> {
    const Block* block_;
    class TIRFunction* parent_;
    ilist<TIRInstruction> instructions_;

    TIRBlock(const Block* block, TIRFunction* parent) noexcept :
        block_(block), parent_(parent) {}

    friend class TIRFunction;

public:
    const ilist<TIRInstruction>& getInstructions() const noexcept {
        return instructions_;
    }

    ilist<TIRInstruction>& getInstructions() noexcept {
        return instructions_;
    }

    TIRFunction* getParent() noexcept {
        return parent_;
    }

    const TIRFunction* getParent() const noexcept {
        return parent_;
    }

    const Block* getBlock() const noexcept {
        return block_;
    }

    void addInstruction(TIRInstruction* tirinst) noexcept {
        instructions_.push_back(tirinst);
    }

    ~TIRBlock() noexcept {
        instructions_.freeUsingDelete();
    }
};

} // namespace inr

#endif // INERTIA_TIR_TIRBLOCK_H
