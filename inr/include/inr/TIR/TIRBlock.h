// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TIRBLOCK_H
#define INERTIA_TIR_TIRBLOCK_H

/// @file TIR/TIRBlock.h
/// @brief Contains the Target IR block class.

#include <inr/IR/Block.h>

namespace inr {

/// @brief Represents a TIR block.
class TIRBlock {
    const Block* block_;

    TIRBlock(const Block* block) noexcept : block_(block) {}

    friend class TIRFunction;

public:
};

} // namespace inr

#endif // INERTIA_TIR_TIRBLOCK_H
