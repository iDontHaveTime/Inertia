// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_TYPE_H
#define INERTIA_IR_TYPE_H

/// @file IR/Type.h
/// @brief Represents a Def type.

#include <inr/ADT/ArrView.h>
#include <inr/Math/FPFormat.h>
#include <inr/Support/Assert.h>

#include <cstdint>
#include <vector>

namespace inr {

class IntType;
class PtrType;
class VoidType;
class BlockType;
class FPType;
class ArrayType;
class FuncType;

/// @brief Base class for all IR types.
class Type {
public:
    enum TypeID : unsigned {
        Integer,
        Pointer,
        Void,
        Block,
        Float,
        Array,
        Function,
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

    /// @brief Returns true if this type is float.
    bool isFloat() const {
        return id_ == Float;
    }

    /// @brief Returns true if this type is a function.
    bool isFunction() const {
        return id_ == Function;
    }

    /// @brief Returns true if this type is a block.
    bool isBlock() const {
        return id_ == Block;
    }

    /// @brief Returns true if this type is an array.
    bool isArray() const {
        return id_ == Array;
    }

    template<typename T>
    const T* as() const = delete;

    template<>
    const IntType* as<IntType>() const {
        inr_assert(isInteger(), "Type casted to integer but wasn't one");
        return (const IntType*)this;
    }

    template<>
    const PtrType* as<PtrType>() const {
        inr_assert(isPointer(), "Type casted to pointer but wasn't one");
        return (const PtrType*)this;
    }

    template<>
    const VoidType* as<VoidType>() const {
        inr_assert(isVoid(), "Type casted to void but wasn't one");
        return (const VoidType*)this;
    }

    template<>
    const BlockType* as<BlockType>() const {
        inr_assert(isBlock(), "Type casted to block but wasn't one");
        return (const BlockType*)this;
    }

    template<>
    const FPType* as<FPType>() const {
        inr_assert(isFloat(), "Type casted to float but wasn't one");
        return (const FPType*)this;
    }

    template<>
    const FuncType* as<FuncType>() const {
        inr_assert(isFunction(), "Type casted to function but wasn't one");
        return (const FuncType*)this;
    }

    template<>
    const ArrayType* as<ArrayType>() const {
        inr_assert(isArray(), "Type casted to array but wasn't one");
        return (const ArrayType*)this;
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

    arrview<const Type*> getArgs() const {
        return args_;
    }

    friend class TypeMapInternal;
};

class ArrayType final : public Type {
    const Type* element_;
    uint64_t size_;

    ArrayType(const Type* element, uint64_t size) :
        Type(Array), element_(element), size_(size) {}

public:
    const Type* getElement() const {
        return element_;
    }

    uint64_t getSize() const {
        return size_;
    }

    friend class TypeMapInternal;
};

} // namespace inr

#endif // INERTIA_IR_TYPE_H
