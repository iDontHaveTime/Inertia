// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_CONTEXT_H
#define INERTIA_IR_CONTEXT_H

/// @file IR/Context.h
/// @brief Contains the context class.

#include <inr/ADT/StrView.h>
#include <inr/IR/Constant.h>
#include <inr/IR/Module.h>
#include <inr/IR/Type.h>

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace inr {

struct PairHash {
    template<typename T, typename Y>
    size_t operator()(const std::pair<T, Y>& p) const noexcept {
        size_t h1 = std::hash<T>{}(p.first);
        size_t h2 = std::hash<Y>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

/// @brief This class is used for providing stable pointers to certain data
/// structures.
///
/// Types can be compared with direct comparison, so for example:
/// getInt(32) == getInt(32) should result to true.
/// And obviously something like getVoid() == getInt(1) should not be true.
/// This class currently is implemented by using a storage for storing unique
/// types, and each unique type is stored in a map based on the type. The
/// integer type is stored using its width. The pointer type is stored using its
/// pointee. But not all types are stored in that storage list, some types like
/// void and function signature are not.
/// Void is allocated in the constructor of the context.
/// Function signatures use a list due to the function signature's keys, since
/// it doesn't have just one key to represent the whole signature.
class InrContext {
    /// @brief Modules that exist under this context.
    std::vector<std::unique_ptr<Module>> modules_;
    /// @brief Void type.
    ///
    /// This is here because void type is not dynamic, there can only be one.
    VoidType voidType_;
    /// @brief Pointer type.
    ///
    /// Same reason as void.
    PointerType ptrType_;
    /// @brief Storage of pointers to different types.
    mutable std::vector<std::unique_ptr<Type>> storage_;
    /// @brief Storage of pointers to constant types.
    mutable std::vector<std::unique_ptr<Constant>> constantStorage_;
    /// @brief Map of integers with the width as the key.
    mutable std::unordered_map<unsigned, const Type*> integers_;
    /// @brief A list of function signatures.
    mutable std::vector<std::unique_ptr<FunctionType>> functionTypes_;
    /// @brief Map of integer constants.
    mutable std::unordered_map<std::pair<const IntegerType*, uint64_t>,
                               ConstantInt*, PairHash>
        intConstants_;

    const IntegerType* i1_ = getInt(1);   ///< Cache i1 (common for bool type).
    const IntegerType* i8_ = getInt(8);   ///< Cache i8 (common integer type).
    const IntegerType* i16_ = getInt(16); ///< Cache i16 (common integer type).
    const IntegerType* i32_ = getInt(32); ///< Cache i32 (common integer type).
    const IntegerType* i64_ = getInt(64); ///< Cache i64 (common integer type).
public:
    InrContext() = default;

    InrContext(const InrContext&) = delete;
    InrContext& operator=(const InrContext&) = delete;

    InrContext(InrContext&&) = default;
    InrContext& operator=(InrContext&&) = default;

    /// @brief Creates a new compilation module.
    /// @param name The name for the module.
    Module* newModule(sview name) {
        return modules_.emplace_back(new Module(name)).get();
    }

    /// @brief Returns the void type.
    /// @return Pointer to the void type.
    const VoidType* getVoid() const noexcept {
        return &voidType_;
    }

    /// @brief Creates a new integer type.
    /// @param width The width of the new integer.
    /// @return Pointer to the integer type.
    const IntegerType* getInt(unsigned width) const {
        auto [it, inserted] = integers_.try_emplace(width, nullptr);
        if(inserted) {
            it->second =
                storage_.emplace_back(std::make_unique<IntegerType>(width))
                    .get();
        }
        return (const IntegerType*)it->second;
    }

    /// @brief Creates a pointer to the provided pointee.
    /// @param pointee The type the pointer points to.
    /// @return Pointer to the pointer type.
    const PointerType* getPointer() const {
        return &ptrType_;
    }

    /// @brief Creates a function signature with the provided return type and
    /// args.
    /// @param returnType The type the function returns.
    /// @param args The types of the arguments of the function.
    /// @return Pointer to the function's signature.
    const FunctionType* getFunction(
        const Type* returnType,
        std::initializer_list<const Type*> args = {}) const {
        for(const auto& ft : functionTypes_) {
            if(ft->getReturn() == returnType &&
               args.size() == ft->getArgs().size() &&
               std::equal(ft->getArgs().begin(), ft->getArgs().end(),
                          args.begin())) {
                return ft.get();
            }
        }

        return functionTypes_
            .emplace_back(std::make_unique<FunctionType>(returnType, args))
            .get();
    }

    /// @brief Shorcut for doing getInt(1).
    /// @return Integer with the width of 1 bits.
    inline const IntegerType* getI1() const {
        return i1_;
    }

    /// @brief Shorcut for doing getInt(8).
    /// @return Integer with the width of 8 bits.
    inline const IntegerType* getI8() const {
        return i8_;
    }

    /// @brief Shorcut for doing getInt(16).
    /// @return Integer with the width of 16 bits.
    inline const IntegerType* getI16() const {
        return i16_;
    }

    /// @brief Shorcut for doing getInt(32).
    /// @return Integer with the width of 32 bits.
    inline const IntegerType* getI32() const {
        return i32_;
    }

    /// @brief Shorcut for doing getInt(64).
    /// @return Integer with the width of 64 bits.
    inline const IntegerType* getI64() const {
        return i64_;
    }

    ConstantInt* getIntConstant(const IntegerType* type, uint64_t value) const {
        auto key = std::make_pair(type, value);
        auto it = intConstants_.find(key);
        if(it != intConstants_.end()) return it->second;

        auto ci = (ConstantInt*)constantStorage_
                      .emplace_back(std::make_unique<ConstantInt>(type, value))
                      .get();
        intConstants_[key] = ci;
        return ci;
    }

    ~InrContext() noexcept = default;
};

} // namespace inr

#endif // INERTIA_IR_CONTEXT_H
