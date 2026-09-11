// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/IR/ArgDef.h>
#include <inr/IR/Builder.h>
#include <inr/IR/Printer.h>
#include <inr/IR/TUnit.h>
#include <inr/IR/TypeMap.h>
#include <inr/IR/Verifier.h>

int main() {
    inr::TUnit unit(__FILE__);
    inr::TypeMap map;
    inr::IRBuilder builder(unit, map);

    inr::FuncBuilder mainBuilder = builder.createFunction(
        map.getFunc(map.getI32(), {map.getI32(), map.getPtr()}, false), "main",
        inr::TypeExt::SignExt);

    mainBuilder.createBlock("entry").createRet(builder.getZeroConst(32));

    mainBuilder.setArgExtAndName(0, inr::TypeExt::SignExt, "argc")
        .setArgName(1, "argv");

    if(!inr::Verifier::verify(unit, &inr::log())) {
        return 1;
    }

    inr::IRPrinter printer(unit);
    printer.print(inr::out());

    return 0;
}
