// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_VALUE_H
#define INERTIA_IR_VALUE_H

/// @file IR/Value.h
/// @brief Provides the value class.

#include <inr/ADT/StrView.h>
#include <inr/IR/Type.h>

namespace inr {

/// @brief Base class for many IR concepts such as instructions, functions,
/// etc..
///
/// Available value types can be seen in the `Value::ValueID` enum class.
class Value {
public:
    /// @brief What type of value it is.
    enum class ValueID : uint8_t {
        Argument,
        Function,
        Block,
        ConstantInt,
        Instruction
    };

    /// @brief Flags for a value.
    enum class ValueFlags : uint32_t {
        None = 0x0,    ///< No flags.
        Volatile = 0x1 ///< Value may have side effects, cannot be optimized
                       ///< away (DCE).
    };

protected:
    /// @brief What type of value it is.
    ValueID valueID_;
    /// @brief What IR type it is.
    const Type* type_;
    /// @brief Name of this value.
    ///
    /// Name is sometimes optional, sometimes not, depends on the `valudID_`
    /// variable.
    sview name_;

    /// @brief Tracks what other values use this one.
    std::vector<Value*> users_;

    /// @brief Flags of this value.
    /// @see `ValueFlags` to see what flags are available.
    uint32_t flags_;

public:
    /// @brief The default constructor for value.
    /// @param id The type of value it is.
    /// @param type The IR type it should be.
    /// @param name Name of the value.
    explicit Value(ValueID id, const Type* type, sview name = {}) noexcept :
        valueID_(id), type_(type), name_(name), flags_(0) {}

    /// @brief Gets the name of the value.
    /// @return String view of the name.
    sview getName() const noexcept {
        return name_;
    }

    /// @brief Sets the name of the value.
    /// @param name The new name.
    void setName(sview name) noexcept {
        name_ = name;
    }

    /// @brief Gets the IR type of the value.
    /// @return The IR type.
    const Type* getType() const noexcept {
        return type_;
    }

    /// @brief Gets the type of the value.
    ValueID getValueID() const noexcept {
        return valueID_;
    }

    /// @brief Sets the IR type of the value.
    /// @param type The new type.
    void setType(const Type* type) noexcept {
        type_ = type;
    }

    void addUser(Value* user) {
        users_.push_back(user);
    }

    void removeUser(Value* user) {
        users_.erase(std::remove(users_.begin(), users_.end(), user),
                     users_.end());
    }

    const std::vector<Value*>& getUsers() const noexcept {
        return users_;
    }

    bool hasUsers() const noexcept {
        return !users_.empty();
    }

    bool isVolatile() const noexcept {
        return flags_ & decltype(flags_)(ValueFlags::Volatile);
    }

    virtual ~Value() noexcept = default;
};

raw_stream& operator<<(raw_stream&, const Value&);

} // namespace inr

#endif // INERTIA_IR_VALUE_H
