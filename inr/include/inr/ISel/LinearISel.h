// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_ISEL_LINEARISEL_H
#define INERTIA_ISEL_LINEARISEL_H

/// @file ISel/LinearISel.
/// @brief Contains the triple based linear ISel class.

#include <inr/PassManager.h>
#include <inr/Target/Triple.h>

namespace inr {

class TripleLinearISel {
public:
    static std::unique_ptr<inr::ISelPass> get(inr::Triple);
};

} // namespace inr

#endif // INERTIA_ISEL_LINEARISEL_H
