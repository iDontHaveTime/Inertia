// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_MIR_MACHINEOPERAND_H
#define INERTIA_MIR_MACHINEOPERAND_H

/// @file MIR/MachineOperand.h
/// @brief Contains the machine operand class.

#include <inr/IR/Constant.h>
#include <inr/MIR/Register.h>

#include <cstdint>
#include <variant>

namespace inr {

/// @brief A single operand of a machine instruction.
class MachineOperand {
public:
    enum class Kind : uint8_t {
        Reg,        ///< Physical or virtual register.
        Imm,        ///< Immediate integer value.
        FrameIndex, ///< Stack slot, resolved to [rbp +/- N] during frame
                    ///< lowering.
        BlockAddr   ///< Basic block address, for branches.
    };

private:
    Kind kind_; ///< Kind of the operand.

    std::variant<Register, const ConstantInt*, uint32_t> data_;

    explicit MachineOperand(Register reg) noexcept :
        kind_(Kind::Reg), data_(reg) {}
    explicit MachineOperand(const ConstantInt* imm) noexcept :
        kind_(Kind::Imm), data_(imm) {}
    explicit MachineOperand(uint32_t idx, Kind kind) noexcept :
        kind_(kind), data_(idx) {}

public:
    static MachineOperand createReg(Register reg) noexcept {
        return MachineOperand(reg);
    }

    static MachineOperand createImm(const ConstantInt* imm) noexcept {
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
        return std::get<Register>(data_);
    }

    const ConstantInt* getImm() const noexcept {
        return std::get<const ConstantInt*>(data_);
    }

    uint32_t getFrameIndex() const noexcept {
        return std::get<uint32_t>(data_);
    }
    uint32_t getBlockAddr() const noexcept {
        return std::get<uint32_t>(data_);
    }
};

} // namespace inr

#endif // INERTIA_MIR_MACHINEOPERAND_H
