// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/IR/ArgDef.h>
#include <inr/IR/InstDef.h>
#include <inr/IR/Linkage.h>
#include <inr/IR/Printer.h>
#include <inr/IR/TUnit.h>
#include <inr/IR/TypeMap.h>
#include <inr/IR/Verifier.h>
#include <inr/Math/BigInt.h>
#include <inr/TIR/Printer.h>
#include <inr/TIR/Translator.h>
#include <inr/Target/Host.h>
#include <inr/Target/TargetDesc.h>

int main() {
    inr::TUnit unit("ArithmeticTest.cpp");
    inr::TypeMap map;

    auto arith_test = unit.createFunction(
        map.getFunc(map.getI32(), {map.getI32(), map.getI32(), map.getI32()},
                    false),
        "arith_test", inr::Linkage::Global, inr::TypeExt::NoExt);
    auto arith_entry = unit.createBlock(map, arith_test, "entry");

    auto arith_add = inr::AddInst::createAdd(arith_entry, arith_test->getArg(0),
                                             arith_test->getArg(1), "add");
    auto arith_sub = inr::SubInst::createSub(arith_entry, arith_add,
                                             arith_test->getArg(2), "sub");

    auto arith_sdiv = inr::SDivInst::createSDiv(arith_entry, arith_sub,
                                                arith_test->getArg(1), "div");
    auto arith_srem = inr::SRemInst::createSRem(arith_entry, arith_sub,
                                                arith_test->getArg(2), "rem");

    auto arith_temp =
        inr::MulInst::createMul(arith_entry, arith_sdiv, arith_srem, "temp");
    auto arith_result =
        inr::AddInst::createAdd(arith_entry, arith_temp, arith_add, "result");

    inr::RetInst::createRet(map, arith_entry, arith_result);

    if(!inr::Verifier::verify(unit, &inr::out())) {
        return 1;
    }

    inr::IRPrinter ir_printer(unit);
    ir_printer.print(inr::out());

    auto info = inr::TargetRegistry::getDesc(inr::host::getTarget());
    if(!info) {
        inr::out() << "Host target not included\n";
        return 0;
    }

    inr::Translator ir_to_tir(info.get());
    auto tir_mod = ir_to_tir.translate(unit);

    inr::TIRPrinter tir_printer(tir_mod, info.get());
    tir_printer.print(inr::out());

    return 0;
}
