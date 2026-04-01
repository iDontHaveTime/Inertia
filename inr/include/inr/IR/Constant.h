// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_CONSTANT_H
#define INERTIA_IR_CONSTANT_H

/// @file IR/Constant.h
/// @brief Contains constant value classes.

#include <inr/ADT/BigInt.h>
#include <inr/IR/Type.h>
#include <inr/IR/Value.h>

#include <cstdint>

namespace inr {

/// @brief This class represents a constant value node.
class Constant : public Value {
public:
    enum class ConstantID : uint8_t { Integer };

    constexpr static ValueID constIDToValueID(ConstantID cid) noexcept {
        switch(cid) {
            case ConstantID::Integer:
                return ValueID::ConstantInt;
        }
    }

protected:
    ConstantID constantID_;

    Constant(ConstantID id, const Type* type, sview name = "") :
        Value(constIDToValueID(id), type, name), constantID_(id) {}

public:
    ConstantID getConstantID() const noexcept {
        return constantID_;
    }

    virtual ~Constant() noexcept = default;
};

/// @brief This is a constant integer value node.
class ConstantInt : public Constant {
    bigint value_;

public:
    ConstantInt(const IntegerType* type, uint64_t value) :
        Constant(ConstantID::Integer, type), value_(type->getWidth(), value) {};

    ConstantInt(const IntegerType* type, bigint value) :
        Constant(ConstantID::Integer, type), value_(std::move(value)) {}

    const bigint& getValue() const noexcept {
        return value_;
    }
};

} // namespace inr

#endif // INERTIA_IR_CONSTANT_H
