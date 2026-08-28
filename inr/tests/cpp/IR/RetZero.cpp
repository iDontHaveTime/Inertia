// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/IR/ArgDef.h>
#include <inr/IR/InstDef.h>
#include <inr/IR/Linkage.h>
#include <inr/IR/TUnit.h>
#include <inr/IR/TypeMap.h>
#include <inr/IR/Verifier.h>
#include <inr/Math/BigInt.h>

int main() {
    inr::TUnit unit(__FILE__);
    inr::TypeMap map;

    auto mainFunction = unit.createFunction(
        map, map.getFunc(map.getI32(), {map.getI32(), map.getPtr()}, false),
        "main", inr::Linkage::Global, inr::TypeExt::SignExt);

    mainFunction->getArg(0)->setName("argc");
    mainFunction->getArg(1)->setName("argv");

    auto entryBlock = unit.createBlock(map, mainFunction, "entry");

    inr::RetInst::createRet(map, entryBlock,
                            unit.createConst(map.getI32(), inr::bigint(32, 0)));

    if(!inr::Verifier::verify(unit, &inr::log())) {
        return 1;
    }

    return 0;
}
