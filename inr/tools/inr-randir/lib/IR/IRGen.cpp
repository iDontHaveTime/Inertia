// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/IR/Type.h>
#include <inr/Support/Assert.h>
#include <randir/IR/IRGen.h>

#include <random>

namespace randir {

uint32_t IRGen::getRand32(uint32_t min, uint32_t max) {
    inr_assert(min <= max, "IRGen getRand32(): max is less than min");
    std::uniform_int_distribution<uint32_t> dist(min, max);
    return dist(rd_);
}

const inr::IntType* IRGen::randomInt(uint32_t max) {
    inr_assert(max >= 1,
               "IRGen randomInt(): integer must be at least 1 bit wide");
    return tm_.getInt(getRand32(1, max));
}

const inr::Type* IRGen::randomBasicType(uint32_t max) {
    while(true) {
        inr::Type::TypeID randomEnum =
            getRandEnum(inr::Type::Integer, inr::Type::Function);
        switch(randomEnum) {
            case inr::Type::Integer:
                return randomInt(max);
            case inr::Type::Pointer:
                return tm_.getPtr();
            case inr::Type::Void:
            case inr::Type::Block:
            case inr::Type::Function:
            case inr::Type::Float:
                break;
        }
    }
}

const inr::Type* IRGen::randomReturnType(uint32_t max) {
    while(true) {
        inr::Type::TypeID randomEnum =
            getRandEnum(inr::Type::Integer, inr::Type::Function);
        switch(randomEnum) {
            case inr::Type::Integer:
                return randomInt(max);
            case inr::Type::Pointer:
                return tm_.getPtr();
            case inr::Type::Void:
                return tm_.getVoid();
            case inr::Type::Block:
                break;
            case inr::Type::Function:
                break;
            case inr::Type::Float:
                break;
        }
    }
}

bool IRGen::getRandBool() {
    return getRand32(0, 1);
}

} // namespace randir
