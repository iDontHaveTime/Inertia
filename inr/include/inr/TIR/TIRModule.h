// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TIRMODULE_H
#define INERTIA_TIR_TIRMODULE_H

/// @file TIR/TIRModule.h
/// @brief Contains the Target IR module class.

#include <inr/ADT/IList.h>
#include <inr/ADT/StrView.h>
#include <inr/IR/Function.h>
#include <inr/IR/Module.h>
#include <inr/TIR/TIRFunction.h>

#include <memory>

namespace inr {

/// @brief Represents a TIR module.
class TIRModule {
    sview name_;
    ilist<TIRFunction> funcs_;

    TIRModule(const Module* mod) noexcept : name_(mod->getName()) {}

public:
    static std::unique_ptr<TIRModule> newModule(const Module* mod) {
        return std::unique_ptr<TIRModule>(new TIRModule(mod));
    }

    TIRFunction* newFunction(const Function* irFunc) {
        return funcs_.push_back(new TIRFunction(irFunc));
    }

    sview getName() const noexcept {
        return name_;
    }

    ~TIRModule() noexcept {
        funcs_.freeUsingDelete();
    }
};

} // namespace inr

#endif // INERTIA_TIR_TIRMODULE_H
