// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_AST_TYPE_H
#define INRCC_AST_TYPE_H

/// @file AST/Type.h
/// @brief Represents a type.

#include <cstdint>
namespace inrcc {

class Type {
public:
    enum TypeID : uint16_t { Pointer, Char, Short, Int, Long, LongLong };
    enum Qualifier : uint8_t { Const = 0x1, Restrict = 0x2, Volatile = 0x4 };

private:
    TypeID id_;
    Qualifier qual_{};

public:
    Type(TypeID id) noexcept : id_(id) {}

    TypeID getID() const noexcept {
        return id_;
    }

    Qualifier getQual() const noexcept {
        return qual_;
    }
};

class PointerType : public Type {
    const Type* pointee_;

public:
    PointerType(const Type* pointee) noexcept :
        Type(Pointer), pointee_(pointee) {}

    const Type* getPointee() const noexcept {
        return pointee_;
    }
};

} // namespace inrcc

#endif // INRCC_AST_TYPE_H
