// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_MIR_MACHINEBLOCK_H
#define INERTIA_MIR_MACHINEBLOCK_H

/// @file MIR/MachineBlock.h
/// @brief Contains the machine block class.

#include <inr/ADT/IList.h>
#include <inr/ADT/StrView.h>
#include <inr/MIR/MachineInst.h>

namespace inr {

/// @brief Lowered block.
class MachineBlock : public ilist_node<MachineBlock> {
    sview name_;                      ///< Usually same as the original block.
    class MachineFunction* parent_;   ///< Used for CCState.
    ilist<MachineInst> instructions_; ///< Made as an ilist for opts.

    /// @brief Creates a new machine block.
    /// @param name Sets the name of it.
    /// @param parent Sets the parent.
    MachineBlock(sview name, MachineFunction* parent) noexcept :
        name_(name), parent_(parent) {}

public:
    /// @brief Returns the stored parent, const.
    const MachineFunction* getParent() const noexcept {
        return parent_;
    }

    /// @brief Returns the stored parent.
    MachineFunction* getParent() noexcept {
        return parent_;
    }

    /// @brief Creates a new machine block.
    /// @note Does not push it to MachineFunction's list.
    static MachineBlock* create(sview name, MachineFunction* parent) {
        return new MachineBlock(name, parent);
    }

    /// @brief Do not use manually.
    MachineInst* push(MachineInst* inst) {
        return instructions_.push_back(inst);
    }

    /// @brief Returns the block's name.
    sview getName() const noexcept {
        return name_;
    }

    /// @brief Returns a const reference to the instruction list.
    const ilist<MachineInst>& getInstructions() const noexcept {
        return instructions_;
    }

    ~MachineBlock() noexcept {
        instructions_.freeUsingDelete();
    }
};

} // namespace inr

#endif // INERTIA_MIR_MACHINEBLOCK_H
