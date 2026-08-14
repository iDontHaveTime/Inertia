// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/IR/BlockDef.h>
#include <inr/IR/FuncDef.h>

namespace inr {

BlockDef* FuncDef::createBlock(const BlockType* bt, std::string_view name) {
    return blocks_.push_back(new BlockDef(bt, name));
}

} // namespace inr
