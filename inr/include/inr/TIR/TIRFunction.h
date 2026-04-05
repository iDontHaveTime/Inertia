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
#include <inr/TIR/TIRBlock.h>

namespace inr {

/// @brief Represents a TIR function.
class TIRFunction : public ilist_node<TIRFunction> {
    const Function* func_;
    ilist<TIRBlock> blocks_;

    TIRFunction(const Function* func) : func_(func) {}

    friend class TIRModule;

public:
    TIRBlock* newBlock(const Block* block) {
        return blocks_.push_back(new TIRBlock(block));
    }

    const Function* getFunction() const noexcept {
        return func_;
    }

    ~TIRFunction() noexcept {
        blocks_.freeUsingDelete();
    }
};

} // namespace inr

#endif // INERTIA_TIR_TIRFUNCTION_H
