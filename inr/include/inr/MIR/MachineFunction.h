// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_MIR_MACHINEFUNCTION_H
#define INERTIA_MIR_MACHINEFUNCTION_H

/// @file MIR/MachineFunction.h
/// @brief Contains the machine function class.

#include <inr/ADT/IList.h>
#include <inr/ADT/IVector.h>
#include <inr/ADT/StrView.h>
#include <inr/IR/Value.h>
#include <inr/MIR/MachineBlock.h>
#include <inr/MIR/Register.h>

namespace inr {

/// @brief A machine level function containing machine blocks.
///
/// Mirrors inr::Function but at the MIR level.
/// Tracks frame slots for frame lowering.
class MachineFunction : public ilist_node<MachineFunction> {
    const class Function* function_;
    class MachineModule* parent_;
    ilist<MachineBlock> blocks_;

    explicit MachineFunction(const Function* function, MachineModule* parent) :
        function_(function), parent_(parent) {}

    friend class ISel;
    friend class MachineModule;

public:
    MachineModule* getParent() const noexcept {
        return parent_;
    }

    MachineModule* getParent() noexcept {
        return parent_;
    }

    const Function* getFunction() const noexcept {
        return function_;
    }

    static MachineFunction* create(const Function* function,
                                   MachineModule* parent) {
        return new MachineFunction(function, parent);
    }

    MachineBlock* newBlock(sview label) {
        return blocks_.push_back(MachineBlock::create(label, this));
    }

    const ilist<MachineBlock>& getBlocks() const noexcept {
        return blocks_;
    }
    ilist<MachineBlock>& getBlocks() noexcept {
        return blocks_;
    }

    ~MachineFunction() noexcept {
        blocks_.freeUsingDelete();
    }
};

} // namespace inr

#endif // INERTIA_MIR_MACHINEFUNCTION_H
