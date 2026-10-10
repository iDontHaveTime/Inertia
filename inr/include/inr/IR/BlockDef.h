// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_BLOCKDEF_H
#define INERTIA_IR_BLOCKDEF_H

/// @file IR/BlockDef.h
/// @brief Represents a basic block.

#include <inr/ADT/IList.h>
#include <inr/IR/Def.h>
#include <inr/IR/InstDef.h>
#include <inr/IR/Type.h>

namespace inr {

class FuncDef;

class BlockDef final : public Def, public ilist_node<BlockDef> {
    FuncDef* parent_;
    ilist<InstDef> instructions_;

    BlockDef(FuncDef* parent, const BlockType* bt, std::string_view name) :
        Def(bt, BlockDefType, name), parent_(parent) {}

    friend class FuncDef;

public:
    ~BlockDef() override {
        instructions_.deleteNodes();
    }

    ilist<InstDef>& getInstructions() {
        return instructions_;
    }

    const ilist<InstDef>& getInstructions() const {
        return instructions_;
    }

    FuncDef* getParent() {
        return parent_;
    }

    const FuncDef* getParent() const {
        return parent_;
    }
};

} // namespace inr

#endif // INERTIA_IR_BLOCKDEF_H
