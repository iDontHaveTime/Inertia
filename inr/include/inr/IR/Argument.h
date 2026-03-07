#ifndef INERTIA_IR_ARGUMENT_H
#define INERTIA_IR_ARGUMENT_H

/// @file IR/Argument.h
/// @brief Contains the argument class used by functions.

#include <inr/ADT/StrView.h>
#include <inr/IR/Value.h>

namespace inr {

/// @brief A function argument class.
class Argument : public Value {
    class Function* parent_;
    unsigned index_;

public:
    Argument(Function* parent, const Type* type, unsigned index,
             sview name = "") noexcept :
        Value(ValueID::Argument, type, name), parent_(parent), index_(index) {}

    unsigned getIndex() const noexcept {
        return index_;
    }

    const Function* getParent() const noexcept {
        return parent_;
    }

    Function* getParent() noexcept {
        return parent_;
    }

    friend class Function;
};

} // namespace inr

#endif // INERTIA_IR_ARGUMENT_H
