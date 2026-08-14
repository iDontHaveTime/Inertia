// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TARGET_TARGET_H
#define INERTIA_TARGET_TARGET_H

/// @file Target/Target.h
/// @brief Provides the target class.

#include <cstdint>
#include <string_view>

namespace inr {

enum class Arch : uint16_t {
    Error,  ///< No architecture.
    x86_64, ///< AMD64 Architecture.
    x86_32, ///< 32-bit x86.
};

enum class OS : uint16_t {
    Unknown, ///< Unknown OS.
    Linux,   ///< Linux.
};

enum class ABI : uint16_t {
    None, ///< No specific ABI.
    GNU,  ///< GNU ABI.
};

std::string_view getArchStr(Arch);
std::string_view getOSStr(OS);
std::string_view getABIStr(ABI);

Arch getStrArch(std::string_view);
OS getStrOS(std::string_view);
ABI getStrABI(std::string_view);

/// @brief Represents a target.
/// @note Similar to LLVM's target triple.
class Target {
    Arch arch_ = Arch::Error;
    OS os_ = OS::Unknown;
    ABI abi_ = ABI::None;

public:
    Target() = default;
    Target(Arch arch, OS os, ABI abi) : arch_(arch), os_(os), abi_(abi) {}

    /// @brief Creates a new triple from the string.
    ///
    /// Follows a similar format to llvm's target triple class.
    /// `arch-os-abi`
    /// So for example x86-64 linux would usually be `x86_64-linux-gnu`.
    Target(std::string_view str);

    static Target fromString(std::string_view str);

    /// @brief Returns this triple as a string.
    std::string str();

    Arch getArch() const {
        return arch_;
    }

    OS getOS() const {
        return os_;
    }

    ABI getABI() const {
        return abi_;
    }
};

} // namespace inr

#endif // INERTIA_TARGET_TARGET_H
