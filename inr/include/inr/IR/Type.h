// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_TYPE_H
#define INERTIA_IR_TYPE_H

/// @file IR/Type.h
/// @brief Represents a Def type.

#include <inr/ADT/ArrView.h>
#include <inr/Math/FPFormat.h>

#include <vector>

namespace inr {

/// @brief Base class for all IR types.
class Type {
public:
    enum TypeID : unsigned {
        Integer, // Integer MUST be first.
        Pointer,
        Void,
        Block,
        Float,
        Function, // Make sure function is last.
    };

private:
    TypeID id_;

protected:
    constexpr Type(TypeID id) : id_(id) {}

public:
    friend class TypeMapInternal;

    Type(const Type&) = default;
    Type& operator=(const Type&) = default;

    Type(Type&&) noexcept = default;
    Type& operator=(Type&&) noexcept = default;

    /// @brief Returns the type of the type.
    TypeID getID() const {
        return id_;
    }

    /// @brief Returns true if this type is a pointer.
    bool isPointer() const {
        return id_ == Pointer;
    }

    /// @brief Returns true if this type is an integer.
    bool isInteger() const {
        return id_ == Integer;
    }

    /// @brief Returns true if this type is void.
    bool isVoid() const {
        return id_ == Void;
    }

    bool isFloat() const {
        return id_ == Float;
    }

    bool isFunction() const {
        return id_ == Function;
    }

    bool isBlock() const {
        return id_ == Block;
    }
};

/// @brief Represents the `void` type.
class VoidType final : public Type {
    constexpr VoidType() : Type(Void) {}

public:
    friend class TypeMapInternal;
};

/// @brief Represents the `ptr` type.
class PtrType final : public Type {
    constexpr PtrType() : Type(Pointer) {}

public:
    friend class TypeMapInternal;
};

/// @brief Represents the `iX` type.
class IntType final : public Type {
    unsigned width_;

    constexpr IntType(unsigned width) : Type(Integer), width_(width) {}

public:
    friend class TypeMapInternal;

    /// @brief Returns the bitwidth of this integer.
    unsigned getWidth() const {
        return width_;
    }
};

class BlockType final : public Type {
    constexpr BlockType() : Type(Block) {}

public:
    friend class TypeMapInternal;
};

class FPType final : public Type {
    FPFormat format_;

    constexpr FPType(FPFormat fmt) : Type(Float), format_(fmt) {}

public:
    friend class TypeMapInternal;

    /// @brief Returns the floating point format.
    FPFormat getFormat() const {
        return format_;
    }

    unsigned getWidth() const {
        switch(format_) {
            case FPFormat::Binary16:
                return 16;
            case FPFormat::Binary32:
                return 32;
            case FPFormat::Binary64:
                return 64;
            case FPFormat::x87_80:
                return 80;
        }
    }
};

class FuncType final : public Type {
    const Type* ret_;
    std::vector<const Type*> args_;
    bool vararg_;

    FuncType(const Type* ret, arrview<const Type*> args, bool vararg) :
        Type(Function),
        ret_(ret),
        args_(args.begin(), args.end()),
        vararg_(vararg) {}

public:
    const Type* getReturn() const {
        return ret_;
    }

    unsigned getNumArgs() const {
        return args_.size();
    }

    const Type* getArg(unsigned i) const {
        return args_[i];
    }

    bool isVararg() const {
        return vararg_;
    }

    friend class TypeMapInternal;
};

} // namespace inr

#endif // INERTIA_IR_TYPE_H
