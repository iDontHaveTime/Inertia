// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TARGET_TRIPLE_H
#define INERTIA_TARGET_TRIPLE_H

/// @file Target/Triple.h
/// @brief Contains the target triple class.

#include <inr/ADT/StrView.h>
#define INR_CCFUNC
#include <inr/Target/CCFunc.h>

#include <cstdint>
#include <string>

namespace inr {

enum class CallingConv {
    C, ///< Chooses the default calling convention.

    SysV ///< SystemV calling convention.
};

/// @brief The target triple class.
///
/// This class represents a target triple, which is in the <arch>-<os>-<abi>
/// format. For example Linux on x86-64 would be x86_64-linux-gnu.
class Triple {
public:
    enum class Arch : uint16_t { Unknown = 0, x86_64 };
    enum class OS : uint16_t { Unknown = 0, Linux };
    enum class ABI : uint16_t { Unknown = 0, GNU };
    /// @brief Returns the triple of the host.
    static Triple getDefaultTriple() noexcept;

    /// @brief Returns the arch as a string.
    static sview getArchStr(Arch) noexcept;
    /// @brief Returns the OS as a string.
    static sview getOSStr(OS) noexcept;
    /// @brief Returns the ABI as a string.
    static sview getABIStr(ABI) noexcept;

    static Triple fromString(sview) noexcept;

private:
    Arch arch_;
    OS os_;
    ABI abi_;

    constexpr Triple() noexcept :
        arch_(Arch::Unknown), os_(OS::Unknown), abi_(ABI::Unknown) {}

public:
    constexpr Triple(const Triple&) = default;
    constexpr Triple& operator=(const Triple&) = default;

    constexpr Triple(Triple&&) = default;
    constexpr Triple& operator=(Triple&&) = default;

    /// @brief Creates a new target triple.
    /// @param arch The architecture of the target.
    /// @param os The target's operating system.
    /// @param abi The target's ABI.
    constexpr Triple(Arch arch, OS os, ABI abi) noexcept :
        arch_(arch), os_(os), abi_(abi) {}

    /// @brief Creates a new triple from the string.
    /// @param str_ String to create from.
    Triple(sview str_) noexcept {
        *this = fromString(str_);
    }

    /// @brief Gets the architecture of the target.
    constexpr Arch getArch() const noexcept {
        return arch_;
    }

    /// @brief Gets the OS of the target.
    constexpr OS getOS() const noexcept {
        return os_;
    }

    /// @brief Gets the ABI of the target.
    constexpr ABI getABI() const noexcept {
        return abi_;
    }

    /// @brief Checks if the triple is valid or not.
    /// @return True if valid, false if not.
    constexpr bool validTriple() const noexcept {
        return arch_ != Arch::Unknown && os_ != OS::Unknown;
    }

    /// @brief Returns arch's pointer size.
    unsigned getPointerWidth() const noexcept;

    /// @brief Returns arch's endian.
    std::endian getEndian() const noexcept;

    /// @brief Returns arch's register info.
    const class RegisterInfo* getRegisterInfo() const noexcept {
        return getRegisterInfo(arch_);
    }

    /// @brief Returns calling convention function for args.
    CCFunc getCCArgs(CallingConv cc) const noexcept {
        return getCCArgs(arch_, os_, abi_, cc);
    }

    /// @brief Returns calling convention function for return.
    CCFunc getCCRet(CallingConv cc) const noexcept {
        return getCCRet(arch_, os_, abi_, cc);
    }

    /// @brief Gets the register info based on the arch.
    static const RegisterInfo* getRegisterInfo(Arch arch) noexcept;

    /// @brief Gets the calling convention's function for args.
    /// @param arch Needed when using C calling convention.
    /// @param os Same as arch.
    /// @param abi Same as arch.
    /// @param cc Calling convention override.
    ///
    /// When the calling convention is set to C it automatically picks the
    /// default calling convention for the said architecture, os, and abi.
    static CCFunc getCCArgs(Arch arch, OS os, ABI abi,
                            CallingConv cc = CallingConv::C) noexcept;

    /// @brief Gets the calling convention's function for args.
    /// @see `getCCArgs(Arch, OS, ABI, CallingConv)` for more info.
    static CCFunc getCCRet(Arch arch, OS os, ABI abi,
                           CallingConv cc = CallingConv::C) noexcept;

    /// @brief Returns the triple as a string.
    std::string str() const;
};

class raw_stream& operator<<(raw_stream&, const Triple&);

} // namespace inr

#endif // INERTIA_TARGET_TRIPLE_H
