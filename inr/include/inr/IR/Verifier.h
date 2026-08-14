// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_VERIFIER
#define INERTIA_IR_VERIFIER

/// @file IR/Verifier.h
/// @brief Provides a way to verify IR correctness.

#include <inr/IR/TUnit.h>
#include <inr/Support/Stream.h>

namespace inr {

class Verifier {
public:
    /// @brief Verifies the correctness of the IR.
    /// @return False if malformed.
    static bool verify(const TUnit& unit, inr::stream* os = nullptr);
};

} // namespace inr

#endif // INERTIA_IR_VERIFIER
