// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TARGET_MANGLING_H
#define INERTIA_TARGET_MANGLING_H

/// @file Target/Mangling.h
/// @brief Defines mangling types.

namespace inr {

enum class Mangling : unsigned char {
    None, ///< Emits symbols as they are.
    ELF,  ///< Follows the ELF conventions.
};

}

#endif // INERTIA_TARGET_MANGLING_H
