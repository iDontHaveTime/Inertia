// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_MATH_FPFORMAT_H
#define INERTIA_MATH_FPFORMAT_H

/// @file Math/FPFormat.h
/// @brief Defines floating point formats.

namespace inr {

/// @brief Defines various floating point formats.
///
/// This enum was based off of this wikipedia page:
/// https://en.wikipedia.org/wiki/Floating-point_arithmetic
enum class FPFormat : unsigned char {
    // Binary formats

    /// @brief IEEE-754 Half precision floating point.
    Binary16,
    /// @brief IEEE-754 Single precision floating point.
    ///
    /// This is usually what the `float` type is in C, although not always, and
    /// can be checked via `__STDC_IEC_559__`.
    Binary32,
    /// @brief IEEE-754 Double precision floating point.
    ///
    /// Explanation is the same as for `Binary32` although this time `double`
    /// and not `float`.
    Binary64,

    // ISA-specific

    /// @brief x86's 80-bit binary format.
    x87_80,

    // More would be added when they are supported.
};

} // namespace inr

#endif // INERTIA_MATH_FPFORMAT_H
