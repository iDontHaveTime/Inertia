// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_ARGUMENT_H
#define INERTIA_IR_ARGUMENT_H

/// @file IR/Argument.h
/// @brief Contains the argument class used by functions.

#include <inr/ADT/StrView.h>
#include <inr/IR/Value.h>

namespace inr {

/// @brief A function argument class.
class Argument : public Value {
    class Function* parent_; ///< Where does the arg belong to.
    unsigned index_; ///< Where does the arg sit, e.g. func(arg0, arg1, ...).

public:
    /// @brief Creates a new argument.
    /// @param parent Function this arg belongs to.
    /// @param type Type of the arg, important for CCState.
    /// @param index What index is this arg.
    Argument(Function* parent, const Type* type, unsigned index,
             sview name = "") noexcept :
        Value(ValueID::Argument, type, name), parent_(parent), index_(index) {}

    /// @brief Gets the index this arg is at.
    unsigned getIndex() const noexcept {
        return index_;
    }

    /// @brief Gets the arg's function, const.
    const Function* getParent() const noexcept {
        return parent_;
    }

    /// @brief Gets the arg's function.
    Function* getParent() noexcept {
        return parent_;
    }

    friend class Function;
};

} // namespace inr

#endif // INERTIA_IR_ARGUMENT_H
