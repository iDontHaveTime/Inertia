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

class MachineBlock : public ilist_node<MachineBlock> {
    sview name_;
    class MachineFunction* parent_;
    ilist<MachineInst> instructions_;

    MachineBlock(sview name, MachineFunction* parent) noexcept :
        name_(name), parent_(parent) {}

public:
    MachineFunction* getParent() const noexcept {
        return parent_;
    }

    MachineFunction* getParent() noexcept {
        return parent_;
    }

    static MachineBlock* create(sview name, MachineFunction* parent) {
        return new MachineBlock(name, parent);
    }

    MachineInst* push(MachineInst* inst) {
        return instructions_.push_back(inst);
    }

    sview getName() const noexcept {
        return name_;
    }

    const ilist<MachineInst>& getInstructions() const noexcept {
        return instructions_;
    }

    ~MachineBlock() noexcept {
        for(MachineInst* i = instructions_.head; i != nullptr;) {
            MachineInst* next = i->next;
            delete i;
            i = next;
        }
    }
};

} // namespace inr

#endif // INERTIA_MIR_MACHINEBLOCK_H
