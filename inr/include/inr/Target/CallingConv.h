// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TARGET_CALLINGCONV_H
#define INERTIA_TARGET_CALLINGCONV_H

/// @file Target/CallingConv.h
/// @brief Calling convention related classes live here.

#include <inr/IR/Type.h>
#include <inr/MIR/Register.h>

#include <cstdint>
#include <variant>

namespace inr {

class CCAssign {
    enum class Info {
        Full, ///< Value fits in the location.

        SignExt, ///< Value sign extended in the location.
        ZeroExt, ///< Value zero extended in the location.
        UdefExt, ///< Value extended but with undefined bits.

        Trunc ///< Value is truncated.

    };

    /// @brief What data is the dest assigned to.
    ///
    /// Can be a register (e.g. RDI arg 1).
    /// Or can also be a frame index (e.g. 8(%rsp)).
    std::variant<Register, int64_t> data_;
    /// @brief Info about the dest.
    Info info_;
    /// @brief Used for extra info (like arg number).
    unsigned valN_;
    /// @brief Type of the source.
    const Type* src_;
    /// @brief Type of the dest.
    const Type* dest_;

    CCAssign(Info info, unsigned valN, const Type* src,
             const Type* dest) noexcept :
        data_(0), info_(info), valN_(valN), src_(src), dest_(dest) {}

public:
    static CCAssign getReg(unsigned valN, const Type* src, Register reg,
                           const Type* dest, Info info) {
        CCAssign as(info, valN, src, dest);
        as.data_ = reg;
        return as;
    }

    static CCAssign getMem(unsigned valN, const Type* src, int64_t off,
                           const Type* dest, Info info) {
        CCAssign as(info, valN, src, dest);
        as.data_ = off;
        return as;
    }

    unsigned getValN() const noexcept {
        return valN_;
    }

    Register getDestReg() const {
        return std::get<Register>(data_);
    }

    int64_t getDestMem() const {
        return std::get<int64_t>(data_);
    }

    const Type* getSrcType() const noexcept {
        return src_;
    }

    const Type* getDestType() const noexcept {
        return dest_;
    }

    Info getInfo() const noexcept {
        return info_;
    }
};

class CCState {};

} // namespace inr

#endif // INERTIA_TARGET_CALLINGCONV_H
