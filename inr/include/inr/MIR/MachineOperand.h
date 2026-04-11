// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_MIR_MACHINEOPERAND_H
#define INERTIA_MIR_MACHINEOPERAND_H

/// @file MIR/MachineOperand.h
/// @brief Contains the machine operand class.

#include <inr/IR/Constant.h>
#include <inr/IR/Global.h>
#include <inr/MIR/Register.h>

#include <cstdint>
#include <variant>

namespace inr {

/// @brief A single operand of a machine instruction.
class MachineOperand {
public:
    enum class Kind : uint8_t {
        Reg,            ///< Physical or virtual register.
        Imm,            ///< Immediate integer value.
        DereferenceReg, ///< Dereferenced register, for example 8(%rsp).
        Global          ///< Global variable.
    };

private:
    Kind kind_; ///< Kind of the operand.

    std::variant<Register, const ConstantInt*, MemOperand, const Global*> data_;

    explicit MachineOperand(Register reg) noexcept :
        kind_(Kind::Reg), data_(reg) {}
    explicit MachineOperand(const ConstantInt* imm) noexcept :
        kind_(Kind::Imm), data_(imm) {}
    explicit MachineOperand(Register reg, int64_t off) noexcept :
        kind_(Kind::DereferenceReg), data_(MemOperand(reg, off)) {}
    explicit MachineOperand(MemOperand mem) noexcept :
        kind_(Kind::DereferenceReg), data_(mem) {}
    explicit MachineOperand(const Global* global) noexcept :
        kind_(Kind::Global), data_(global) {}

public:
    static MachineOperand createReg(Register reg) noexcept {
        return MachineOperand(reg);
    }

    static MachineOperand createImm(const ConstantInt* imm) noexcept {
        return MachineOperand(imm);
    }

    static MachineOperand createMem(Register reg, int64_t offset) noexcept {
        return MachineOperand(reg, offset);
    }

    static MachineOperand createMem(MemOperand mem) noexcept {
        return MachineOperand(mem);
    }

    static MachineOperand createGlobal(const Global* global) noexcept {
        return MachineOperand(global);
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

    MemOperand getMem() const noexcept {
        return std::get<MemOperand>(data_);
    }

    const Global* getGlobal() const noexcept {
        return std::get<const Global*>(data_);
    }
};

} // namespace inr

#endif // INERTIA_MIR_MACHINEOPERAND_H
