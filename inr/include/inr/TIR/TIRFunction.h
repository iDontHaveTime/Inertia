// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TIRFUNCTION_H
#define INERTIA_TIR_TIRFUNCTION_H

/// @file TIR/TIRFunction.h
/// @brief Contains the Target IR function class.

#include <inr/ADT/IList.h>
#include <inr/ADT/StrView.h>
#include <inr/IR/Block.h>
#include <inr/IR/Function.h>
#include <inr/IR/Value.h>
#include <inr/TIR/TIRBlock.h>
#include <inr/TIR/TIRInstruction.h>

namespace inr {

/// @brief Represents a TIR function.
class TIRFunction : public ilist_node<TIRFunction> {
    const Function* func_;   ///< Original function.
    ilist<TIRBlock> blocks_; ///< List of blocks.
    TIROperand* frameReg_ = nullptr;
    uint32_t stackSize_ = 0;
    unsigned vregC_; ///< Current vreg count.
    bool needsFP_ = false;
    bool stackKnown_ = true;

    /// @brief Constructs a function.
    ///
    /// Only modules can construct functions.
    TIRFunction(const Function* func) : func_(func) {}

    friend class TIRModule;
    friend class TIRLowering;

public:
    /// @brief Allocates and pushes back a new block.
    TIRBlock* newBlock(const Block* block) {
        return blocks_.push_back(new TIRBlock(block, this));
    }

    /// @brief Returns the original SSA function.
    const Function* getFunction() const noexcept {
        return func_;
    }

    /// @brief Returns a const reference to the block ilist.
    const ilist<TIRBlock>& getBlocks() const noexcept {
        return blocks_;
    }

    /// @brief Returns a reference to the block ilist.
    ilist<TIRBlock>& getBlocks() noexcept {
        return blocks_;
    }

    /// @brief Returns the vregC and increments it.
    unsigned allocateVreg() noexcept {
        return vregC_++;
    }

    ~TIRFunction() noexcept {
        blocks_.freeUsingDelete();
    }
};

} // namespace inr

#endif // INERTIA_TIR_TIRFUNCTION_H
