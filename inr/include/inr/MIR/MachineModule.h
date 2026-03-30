// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_MIR_MACHINEMODULE_H
#define INERTIA_MIR_MACHINEMODULE_H

/// @file MIR/MachineModule.h
/// @brief Contains the machine module class.

#include <inr/ADT/IList.h>
#include <inr/ADT/StrView.h>
#include <inr/MIR/MachineFunction.h>

namespace inr {

/// @brief A machine level module containing machine functions.
///
/// Mirrors inr::Module but at the MIR level.
class MachineModule {
    sview name_;
    ilist<MachineFunction> functions_;

public:
    explicit MachineModule(sview name) : name_(name) {}

    MachineFunction* newFunction(sview name) {
        return functions_.push_back(MachineFunction::create(name, this));
    }

    sview getName() const noexcept {
        return name_;
    }

    const ilist<MachineFunction>& getFunctions() const noexcept {
        return functions_;
    }

    ilist<MachineFunction>& getFunctions() noexcept {
        return functions_;
    }

    void print(raw_stream&, const class TreeNodeBuilder&) const;

    ~MachineModule() noexcept {
        for(MachineFunction* f = functions_.front(); f != nullptr;) {
            MachineFunction* next = f->getNext();
            delete f;
            f = next;
        }
    }
};

} // namespace inr

#endif // INERTIA_MIR_MACHINEMODULE_H
