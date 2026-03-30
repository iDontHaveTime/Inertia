// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_MIR_REGISTER_H
#define INERTIA_MIR_REGISTER_H

/// @file MIR/Register.h
/// @brief Contains the register class.

#include <cstdint>

namespace inr {

/// @brief Represents a physical or virtual register.
class Register {
public:
    enum class Kind : uint8_t { Physical, Virtual };

private:
    uint32_t index_;
    Kind kind_;

public:
    constexpr Kind getKind() const noexcept {
        return kind_;
    }

    constexpr bool isVirtual() const noexcept {
        return kind_ == Kind::Virtual;
    }

    constexpr bool isPhysical() const noexcept {
        return kind_ == Kind::Physical;
    }

    constexpr uint32_t getIndex() const noexcept {
        return index_;
    }

    constexpr Register(uint32_t index, Kind kind) noexcept :
        index_(index), kind_(kind) {}

    constexpr static Register createPhysical(uint32_t index) noexcept {
        return Register(index, Kind::Physical);
    }

    constexpr static Register createVirtual(uint32_t index) noexcept {
        return Register(index, Kind::Virtual);
    }

    constexpr Register(const Register&) noexcept = default;
    constexpr Register& operator=(const Register&) noexcept = default;

    constexpr Register(Register&&) noexcept = default;
    constexpr Register& operator=(Register&&) noexcept = default;
};

} // namespace inr

#endif // INERTIA_MIR_REGISTER_H
