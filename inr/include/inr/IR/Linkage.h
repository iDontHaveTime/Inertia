// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_LINKAGE_H
#define INERTIA_IR_LINKAGE_H

/// @file IR/Linkage.h
/// @brief Represents a linkage type.

#include <cstdint>

namespace inr {

/// @brief Used by global def to represent the linkage type.
enum class Linkage : uint8_t {
    /// @brief External linkage.
    ///
    /// Only one symbol with that name can be defined if it has a global linkage
    /// type.
    Global,
    /// @brief Internal linkage.
    ///
    /// Symbols that are visible only to this `TUnit`, there may be more than
    /// one symbols with the same name defined with this linkage type.
    Local,
    /// @brief Weak linkage.
    ///
    /// If a symbol with the same name is present with a higher linkage type
    /// (global, local, ...) is present, it will override this one.
    Weak,
};

} // namespace inr

#endif // INERTIA_IR_LINKAGE_H
