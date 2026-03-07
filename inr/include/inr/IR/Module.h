#ifndef INERTIA_IR_MODULE_H
#define INERTIA_IR_MODULE_H

/// @file IR/Module.h
/// @brief Contains the module class.

#include <inr/ADT/IList.h>
#include <inr/ADT/StrView.h>
#include <inr/IR/Function.h>
#include <inr/IR/Type.h>
#include <inr/Support/Stream.h>

namespace inr {

/// @brief The module class for the IR.
class Module {
    /// @brief Name of the module.
    sview name_;
    /// @brief Functions inside the module.
    ilist<Function> functions_;

    Module(sview name) noexcept : name_(name) {}

public:
    Function* newFunction(sview name, const FunctionType* signature) {
        return functions_.push_back(new Function(signature, name));
    }

    ~Module() noexcept {
        for(Function* f = functions_.head; f != nullptr;) {
            Function* next = f->next;
            delete f;
            f = next;
        }
    }

    void print(raw_stream&) const;

    friend class InrContext;
};

} // namespace inr

#endif // INERTIA_IR_MODULE_H
