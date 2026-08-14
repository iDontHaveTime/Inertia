// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TOPERAND_H
#define INERTIA_TIR_TOPERAND_H

/// @file TIR/TOperand.h
/// @brief Provides a TIR operand class.

#include <inr/Math/BigInt.h>
#include <inr/Support/Assert.h>

#include <cstdint>

namespace inr {

class TOperand {
public:
    enum OperandKind : unsigned char {
        Register,   ///< Target register.
        VRegister,  ///< Virtual register.
        Immediate,  ///< Immediate.
        BImmediate, ///< Bigint immediate.
    };

private:
    OperandKind kind_;
    union {
        uint32_t regN_;
        int64_t imm_;
        const bigint* bimm_;
    };

    TOperand(OperandKind kind) : kind_(kind) {}
    TOperand(OperandKind kind, uint32_t regN) : kind_(kind), regN_(regN) {}
    TOperand(OperandKind kind, int64_t val) : kind_(kind), imm_(val) {}
    TOperand(const bigint* bi) : kind_(BImmediate), bimm_(bi) {}

public:
    TOperand(const TOperand&) = default;
    TOperand& operator=(const TOperand&) = default;

    TOperand(TOperand&&) noexcept = default;
    TOperand& operator=(TOperand&&) noexcept = default;

    ~TOperand() = default;

    static TOperand createReg(uint32_t regN) {
        return TOperand(Register, regN);
    }

    static TOperand createVReg(uint32_t regN) {
        return TOperand(VRegister, regN);
    }

    static TOperand createImm(int64_t val) {
        return TOperand(Immediate, val);
    }

    static TOperand createBImm(const bigint* bi) {
        return TOperand(bi);
    }

    OperandKind getKind() const {
        return kind_;
    }

    uint32_t getRegN() const {
        inr_assert(kind_ == Register || kind_ == VRegister,
                   "TOperand getRegN(): operand is not a register");
        return regN_;
    }

    int64_t getImm() const {
        inr_assert(kind_ == Immediate,
                   "TOperand getImm(): operand is not an immediate");
        return imm_;
    }

    const bigint* getBImm() const {
        inr_assert(kind_ == BImmediate,
                   "TOperand getBImm(): operand is not a bigint immediate");
        return bimm_;
    }
};

} // namespace inr

#endif // INERTIA_TIR_TOPERAND_H
