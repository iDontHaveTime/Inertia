// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_ALIGN_H
#define INERTIA_SUPPORT_ALIGN_H

/// @file Support/Align.h
/// @brief Provides alignment related functions and classes.

#include <bit>
#include <climits>
#include <cstdint>

namespace inr {

constexpr bool isValidAlign(uint32_t n) noexcept {
    return n > 0 && (n & (n - 1)) == 0;
}

class Alignment {
    uint32_t align_ = 1;

public:
    constexpr Alignment(uint32_t alignment) noexcept :
        align_(std::bit_ceil(alignment)) {}

    constexpr uint32_t getAlignment() const noexcept {
        return align_;
    }

    constexpr void setAlignment(uint32_t alignment) noexcept {
        align_ = std::bit_ceil(alignment);
    }

    constexpr uint32_t getPow2() const noexcept {
        return std::countr_zero(align_);
    }

    constexpr static Alignment fromBits(uint32_t bits) noexcept {
        return Alignment(std::bit_ceil(bits) / CHAR_BIT);
    }

    constexpr Alignment& operator=(uint32_t alignment) noexcept {
        setAlignment(alignment);
        return *this;
    }

    /// @brief Aligns the number to this alignment.
    template<std::unsigned_integral T>
    constexpr T align(T n) const noexcept {
        return (n + align_ - 1) & ~(align_ - 1);
    }

    constexpr Alignment() noexcept = default;

    constexpr Alignment(const Alignment&) noexcept = default;
    constexpr Alignment& operator=(const Alignment&) noexcept = default;

    constexpr Alignment(Alignment&&) noexcept = default;
    constexpr Alignment& operator=(Alignment&&) noexcept = default;

    constexpr ~Alignment() noexcept = default;
};

} // namespace inr

#endif // INERTIA_SUPPORT_ALIGN_H
