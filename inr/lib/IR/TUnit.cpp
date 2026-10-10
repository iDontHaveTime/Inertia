// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/IR/ArgDef.h>
#include <inr/IR/ConstDef.h>
#include <inr/IR/FuncDef.h>
#include <inr/IR/TUnit.h>
#include <inr/IR/UnDef.h>
#include <inr/Support/Assert.h>

namespace inr {

FuncDef* TUnit::createFunction(const FuncType* type, std::string_view name,
                               Linkage linkage, TypeExt retExt) {
    return funcs_.push_back(
        new FuncDef(this, getTypeMap().getPtr(), type, name, linkage, retExt));
}

ConstDef* TUnit::createConst(bigint val) {
    unsigned bitCount = val.getBits();
    return (ConstDef*)defStorage_.own(
        new ConstDef(tmap_.getInt(bitCount), std::move(val)));
}

UnDef* TUnit::createUndef(const Type* t) {
    inr_assert(t != nullptr, "TUnit createUndef(): Type is nullptr");
    return (UnDef*)defStorage_.own(new UnDef(t));
}

} // namespace inr
