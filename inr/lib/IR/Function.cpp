// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/IR/Context.h>
#include <inr/IR/Function.h>

namespace inr {

Block* Function::newBlock(const InrContext& ctx, sview name) {
    return blocks_.push_back(new Block(ctx.getBlock(), this, name));
}

} // namespace inr