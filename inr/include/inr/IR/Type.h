// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_TYPE_H
#define INERTIA_IR_TYPE_H

/// @file IR/Type.h
/// @brief Contains the type class.

#include <cstdint>
#include <initializer_list>
#include <vector>

namespace inr {

/// @brief The base class for all types.
class Type {
public:
    enum class TypeID : uint8_t { Void, Integer, Pointer, Function };

protected:
    TypeID typeID_;

    Type(TypeID id) noexcept : typeID_(id) {}

public:
    TypeID getTypeID() const noexcept {
        return typeID_;
    }

    virtual ~Type() noexcept = default;
};

class VoidType : public Type {
public:
    VoidType() noexcept : Type(TypeID::Void) {}
};

class IntegerType : public Type {
    /// @brief Width of the integer in bits.
    unsigned width_;

public:
    IntegerType(unsigned width) noexcept :
        Type(TypeID::Integer), width_(width) {}

    unsigned getWidth() const noexcept {
        return width_;
    }
};

class FunctionType : public Type {
    /// @brief Return type of the function.
    const Type* return_;
    /// @brief Types of the args of the function.
    std::vector<const Type*> args_;

public:
    FunctionType(const Type* retType,
                 std::initializer_list<const Type*> argTypes) :
        Type(TypeID::Function), return_(retType), args_(argTypes) {}

    const Type* getReturn() const noexcept {
        return return_;
    }

    const std::vector<const Type*>& getArgs() const noexcept {
        return args_;
    }
};

class PointerType : public Type {
    const Type* pointee_;

public:
    PointerType(const Type* pointee) noexcept :
        Type(TypeID::Pointer), pointee_(pointee) {}

    const Type* getPointee() const noexcept {
        return pointee_;
    }
};

/// @brief Type query used for querying a type before context exists.
class TypeQuery {
    Type::TypeID id_;
    union {
        unsigned width_;
    };

public:
    constexpr TypeQuery(Type::TypeID id, unsigned width) noexcept :
        id_(id), width_(width) {}

    constexpr unsigned getWidth() const noexcept {
        return width_;
    }

    constexpr Type::TypeID getID() const noexcept {
        return id_;
    }

    constexpr bool operator==(const TypeQuery& other) const noexcept {
        if(id_ != other.id_) return false;

        switch(id_) {
            case Type::TypeID::Void:
                return true;
            case Type::TypeID::Integer:
                return width_ == other.width_;
            case Type::TypeID::Pointer:
                [[fallthrough]];
            case Type::TypeID::Function:
                return false;
        }
    }
};

/// @brief Be able to print out the type to a stream.
class raw_stream& operator<<(raw_stream&, const Type&);

} // namespace inr

#endif // INERTIA_IR_TYPE_H
