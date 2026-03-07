#ifndef INERTIA_IR_VALUE_H
#define INERTIA_IR_VALUE_H

/// @file IR/Value.h
/// @brief Provides the value class.

#include <inr/ADT/StrView.h>
#include <inr/IR/Type.h>

namespace inr {

class Value {
public:
    enum class ValueID : uint8_t {
        Argument,
        Function,
        ConstantInt,
        Instruction
    };

protected:
    ValueID valueID_;
    const Type* type_;
    sview name_;

public:
    explicit Value(ValueID id, const Type* type, sview name = {}) noexcept :
        valueID_(id), type_(type), name_(name) {}

    sview getName() const noexcept {
        return name_;
    }

    void setName(sview name) noexcept {
        name_ = name;
    }

    const Type* getType() const noexcept {
        return type_;
    }

    ValueID getValueID() const noexcept {
        return valueID_;
    }

    void setType(const Type* type) noexcept {
        type_ = type;
    }
};

raw_stream& operator<<(raw_stream&, const Value&);

} // namespace inr

#endif // INERTIA_IR_VALUE_H
