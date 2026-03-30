// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_BLOCK_H
#define INERTIA_IR_BLOCK_H

/// @file IR/Block.h
/// @brief Contains the block class.

#include <inr/ADT/IList.h>
#include <inr/IR/Instruction.h>
#include <inr/IR/Value.h>

namespace inr {

class Block : public ilist_node<Block> {
    class Function* parent_;
    sview name_;
    ilist<Instruction> instructions_;

    Block(Function* parent, sview name) noexcept :
        parent_(parent), name_(name) {}

public:
    const ilist<Instruction>& getInstructions() const noexcept {
        return instructions_;
    }

    ilist<Instruction>& getInstructions() noexcept {
        return instructions_;
    }

    const Function* getParent() const noexcept {
        return parent_;
    }

    Function* getParent() noexcept {
        return parent_;
    }

    sview getName() const noexcept {
        return name_;
    }

    ~Block() noexcept {
        for(Instruction* i = instructions_.front(); i != nullptr;) {
            Instruction* next = i->getNext();
            delete i;
            i = next;
        }
    }

    friend class Instruction;
    friend class Function;
    friend class Module;
};

} // namespace inr

#endif // INERTIA_IR_BLOCK_H
