#ifndef INERTIA_MIR_MACHINEINST_H
#define INERTIA_MIR_MACHINEINST_H

/// @file MIR/MachineInst.h
/// @brief Contains the machine instruction class.

#include <inr/ADT/IList.h>
#include <inr/MIR/MachineOperand.h>

#include <cstdint>
#include <vector>

namespace inr {

/// @brief A single target machine instruction.
///
/// The opcode is stored as a uint32_t to match the underlying
/// type of target specific opcode enums. Target specific ISel
/// is responsible for constructing these with the correct opcode.
class MachineInst : public ilist_node<MachineInst> {
    uint32_t op_;
    class MachineBlock* parent_;
    std::vector<MachineOperand> operands_;

    MachineInst(uint32_t op, MachineBlock* parent,
                std::vector<MachineOperand> operands) noexcept :
        op_(op), parent_(parent), operands_(std::move(operands)) {}

public:
    static MachineInst* create(uint32_t op, MachineBlock* parent,
                               std::vector<MachineOperand> operands = {}) {
        return new MachineInst(op, parent, std::move(operands));
    }

    uint32_t getOp() const noexcept {
        return op_;
    }

    const std::vector<MachineOperand>& getOperands() const noexcept {
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
