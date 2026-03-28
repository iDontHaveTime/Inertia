// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_MIR_MACHINEINST_H
#define INERTIA_MIR_MACHINEINST_H

/// @file MIR/MachineInst.h
/// @brief Contains the machine instruction class.

#include <inr/ADT/IList.h>
#include <inr/ADT/IVector.h>
#include <inr/MIR/MachineOperand.h>
#include <inr/MIR/Register.h>

#include <cstdint>

namespace inr {

/// @brief A single target machine instruction.
///
/// The opcode is stored as a uint32_t to match the underlying
/// type of target specific opcode enums. Target specific ISel
/// is responsible for constructing these with the correct opcode.
class MachineInst : public ilist_node<MachineInst> {
    uint32_t op_;
    class MachineBlock* parent_;
    ivec<MachineOperand, 3> operands_;

    MachineInst(uint32_t op, MachineBlock* parent,
                ivec<MachineOperand, 3> operands) noexcept :
        op_(op), parent_(parent), operands_(std::move(operands)) {}

public:
    static MachineInst* create(uint32_t op, MachineBlock* parent,
                               ivec<MachineOperand, 3> operands = {});

    MachineBlock* getParent() const noexcept {
        return parent_;
    }

    MachineBlock* getParent() noexcept {
        return parent_;
    }

    uint32_t getOp() const noexcept {
        return op_;
    }

    const ivec<MachineOperand, 3>& getOperands() const noexcept {
        return operands_;
    }

    MachineOperand getOperand(unsigned i) const {
        return operands_.at(i);
    }

    unsigned getNumOperands() const noexcept {
        return operands_.size();
    }
};

} // namespace inr

#endif // INERTIA_MIR_MACHINEINST_H
