// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_IRGEN
#define INERTIA_IR_IRGEN

/// @file IR/IRGen.h
/// @brief Provides the tools to randomly generate IR.

#include <inr/IR/ArgDef.h>
#include <inr/IR/Linkage.h>
#include <inr/IR/Type.h>
#include <inr/IR/TypeMap.h>

#include <cstdint>
#include <random>

namespace randir {

class IRGen {
    std::mt19937 rd_{std::random_device{}()};
    inr::TypeMap& tm_;

public:
    IRGen(inr::TypeMap& tm) : tm_(tm) {}

    template<typename E>
    E getRandEnum(E first, E last) {
        return (E)getRand32(uint32_t(first), uint32_t(last));
    }

    inr::Linkage getRandLinkage() {
        static_assert((unsigned)inr::Linkage::Global == 0,
                      "Global linkage must be first");
        return getRandEnum(inr::Linkage::Global, inr::Linkage::Weak);
    }

    inr::TypeExt getRandExt() {
        static_assert((unsigned)inr::TypeExt::NoExt == 0,
                      "NoExt must be first");
        return getRandEnum(inr::TypeExt::SignExt, inr::TypeExt::ZeroExt);
    }

    bool getRandBool();
    uint32_t getRand32(uint32_t min, uint32_t max);
    const inr::IntType* randomInt(uint32_t max);
    /// @brief Random basic type.
    /// @note Types that are basic are integer, pointer, and floats.
    const inr::Type* randomBasicType(uint32_t max);
    const inr::Type* randomReturnType(uint32_t max);
};

} // namespace randir

#endif // INERTIA_IR_IRGEN
