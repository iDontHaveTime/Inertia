// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_MATH_ALIGN_H
#define INERTIA_MATH_ALIGN_H

/// @file Math/Align.h
/// @brief Provides a class to represent alignment.

#include <inr/Support/Assert.h>

#include <bit>
#include <cstdint>

namespace inr {

/// @brief Represents a power of 2 alignment.
class Align {
    uint8_t pow2_{};

public:
    Align() = default;
    Align(uint8_t pow2) : pow2_(pow2) {}

    Align(const Align&) = default;
    Align& operator=(const Align&) = default;

    Align(Align&&) noexcept = default;
    Align& operator=(Align&&) noexcept = default;

    ~Align() = default;

    /// @brief Returns the power of two of this alignment.
    /// @note To get the byte alignment number it would be `1 << getPow2()`.
    uint8_t getPow2() const {
        return pow2_;
    }

    /// @brief Returns whether or not it has alignment.
    /// @note Both 0 and 1 mean no alignment.
    bool hasAlignment() const {
        return pow2_ <= 1;
    }

    /// @brief Returns whether or not the power of 2 is zero.
    bool isZero() const {
        return pow2_;
    }

    /// @brief Returns `uint32_t(1) << getPow2()`.
    uint32_t getAsValue() const {
        return uint32_t(1) << pow2_;
    }

    /// @brief Makes a new alignment from the pow2.
    static Align fromPow2(uint8_t pow2) {
        return Align(pow2);
    }

    /// @brief From a value, does not allow non power of 2 values.
    template<std::unsigned_integral T>
    static Align fromValue(T val) {
        inr_assert(std::has_single_bit(val),
                   "Align fromValue(): the value must be a power of 2");
        if(!val) return Align();
        return Align(std::countr_zero(val));
    }

    /// @brief Creates a power of 2 rounded up.
    ///
    /// For example 63 would be 64 (2^6).
    template<std::unsigned_integral T>
    static Align roundedUp(T val) {
        return std::bit_ceil(val);
    }

    /// @brief Creates a power of 2 rounded down.
    template<std::unsigned_integral T>
    static Align roundedDown(T val) {
        return std::bit_floor(val);
    }
};

} // namespace inr

#endif // INERTIA_MATH_ALIGN_H
