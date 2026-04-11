// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_INSTIT_H
#define INERTIA_TIR_INSTIT_H

/// @file TIR/InstIt.h
/// @brief Provides the instruction iterator class.

#include <inr/TIR/TIRBlock.h>

namespace inr {

class InstructionIterator {
    TIRBlock* block_;

    class Iterator {
        TIRBlock* block_;
        TIRInstruction* instr_;

    public:
        Iterator() noexcept : block_(nullptr), instr_(nullptr) {}

        Iterator(TIRBlock* block) noexcept :
            block_(block), instr_(block->getInstructions().front()) {}

        Iterator& operator++() noexcept {
            if(block_) {
                if(instr_) {
                    instr_ = instr_->getNext();
                }
                if(!instr_) {
                    block_ = block_->getNext();
                    instr_ =
                        block_ ? block_->getInstructions().front() : nullptr;
                }
            }
            return *this;
        }

        TIRInstruction& operator*() noexcept {
            return *instr_;
        }

        bool operator!=(const Iterator& it) const noexcept {
            return block_ != it.block_;
        }
    };

public:
    InstructionIterator(TIRBlock* block) noexcept : block_(block) {}

    Iterator begin() noexcept {
        return Iterator(block_);
    }

    Iterator end() noexcept {
        return Iterator();
    }
};

} // namespace inr

#endif // INERTIA_TIR_INSTIT_H
