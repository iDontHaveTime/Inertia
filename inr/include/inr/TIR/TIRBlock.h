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
    const Block* block_;        ///< Original block.
    class TIRFunction* parent_; ///< The function is this block is under.
    ilist<TIRInstruction> instructions_; ///< Instructions in this block.

    /// @brief Constructs a TIR block.
    ///
    /// Only functions can create blocks, thus the friend class below.
    TIRBlock(const Block* block, TIRFunction* parent) noexcept :
        block_(block), parent_(parent) {}

    friend class TIRFunction;

public:
    /// @brief Returns a const reference to the instructions.
    const ilist<TIRInstruction>& getInstructions() const noexcept {
        return instructions_;
    }

    /// @brief Returns a reference to the instructions ilist.
    ilist<TIRInstruction>& getInstructions() noexcept {
        return instructions_;
    }

    /// @brief Returns the function this block is under.
    TIRFunction* getParent() noexcept {
        return parent_;
    }

    /// @brief Returns a const pointer to the function this block is under.
    const TIRFunction* getParent() const noexcept {
        return parent_;
    }

    /// @brief Returns the SSA block it came from.
    const Block* getBlock() const noexcept {
        return block_;
    }

    /// @brief Pushes back the instruction to the list.
    void addInstruction(TIRInstruction* tirinst) noexcept {
        instructions_.push_back(tirinst);
    }

    ~TIRBlock() noexcept {
        instructions_.freeUsingDelete();
    }
};

} // namespace inr

#endif // INERTIA_TIR_TIRBLOCK_H
