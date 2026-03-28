// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_MIR_MACHINEOPERAND_H
#define INERTIA_MIR_MACHINEOPERAND_H

/// @file MIR/MachineOperand.h
/// @brief Contains the machine operand class.

#include <inr/MIR/Register.h>

#include <cstdint>

namespace inr {

/// @brief A single operand of a machine instruction.
class MachineOperand {
public:
    enum class Kind : uint8_t {
        Reg,        ///< Physical or virtual register.
        Imm,        ///< Immediate integer value.
        FrameIndex, ///< Stack slot, resolved to [rbp - N] during frame
                    ///< lowering.
        BlockAddr   ///< Basic block address, for branches.
    };

private:
    Kind kind_; ///< Kind of the operand.

    union {
        Register reg_; ///< Register operand.
        uint64_t imm_; ///< Immediate operand.
        uint32_t idx_; ///< Can be either block index, or stack slot
    };

    explicit MachineOperand(Register reg) noexcept :
        kind_(Kind::Reg), reg_(reg) {}
    explicit MachineOperand(uint64_t imm) noexcept :
        kind_(Kind::Imm), imm_(imm) {}
    explicit MachineOperand(uint32_t idx, Kind kind) noexcept :
        kind_(kind), idx_(idx) {}

public:
    static MachineOperand createReg(Register reg) noexcept {
        return MachineOperand(reg);
    }

    static MachineOperand createImm(uint64_t imm) noexcept {
        return MachineOperand(imm);
    }

    static MachineOperand createFrameIndex(uint32_t offset) noexcept {
        return MachineOperand(offset, Kind::FrameIndex);
    }

    static MachineOperand createBlockAddr(uint32_t idx) noexcept {
        return MachineOperand(idx, Kind::BlockAddr);
    }

    Kind getKind() const noexcept {
        return kind_;
    }

    Register getReg() const noexcept {
        return reg_;
    }

    uint64_t getImm() const noexcept {
        return imm_;
    }

    uint32_t getFrameIndex() const noexcept {
        return idx_;
    }
    uint32_t getBlockAddr() const noexcept {
        return idx_;
    }
};

} // namespace inr

#endif // INERTIA_MIR_MACHINEOPERAND_H
