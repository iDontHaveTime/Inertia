// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_TYPEMAP
#define INERTIA_IR_TYPEMAP

/// @file IR/TypeMap.h
/// @brief Provides a map to provide types.

#include <inr/ADT/ArrView.h>
#include <inr/IR/Type.h>
#include <inr/Math/FPFormat.h>

namespace inr {

class TypeMapInternal;

/// @brief Provides unique types to allow pointer comparisons.
class TypeMap {
    TypeMapInternal* internal_;

public:
    TypeMap();

    TypeMap(const TypeMap&) = delete;
    TypeMap& operator=(const TypeMap&) = delete;

    TypeMap(TypeMap&& other) noexcept;
    TypeMap& operator=(TypeMap&&) noexcept;

    ~TypeMap();

    /// @brief Returns an integer type that is 1 bits wide.
    const IntType* getI1();
    /// @brief Returns an integer type that is 8 bits wide.
    const IntType* getI8();
    /// @brief Returns an integer type that is 16 bits wide.
    const IntType* getI16();
    /// @brief Returns an integer type that is 32 bits wide.
    const IntType* getI32();
    /// @brief Returns an integer type that is 64 bits wide.
    const IntType* getI64();

    /// @brief Returns an integer type with the provided width.
    /// @param width Number of bits of the integer.
    const IntType* getInt(unsigned width);

    /// @brief Returns a void type.
    const VoidType* getVoid();
    /// @brief Returns a pointer type.
    const PtrType* getPtr();
    /// @brief Returns a block type.
    const BlockType* getBlock();

    /// @brief Returns a function type.
    /// @param ret Return type of the function.
    /// @param args The types of the function args.
    /// @param vararg Is the function variadic.
    const FuncType* getFunc(const Type* ret, arrview<const Type*> args,
                            bool vararg);

    /// @brief Returns the floating point type with the provided format.
    const FPType* getFloat(FPFormat fmt);
};

} // namespace inr

#endif // INERTIA_IR_TYPEMAP
