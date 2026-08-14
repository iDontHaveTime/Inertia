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
#include <inr/Math/FPFormat.h>
#include <inr/TIR/Printer.h>
#include <inr/TIR/Translator.h>
#include <inr/Target/Host.h>
#include <inr/Target/TargetDesc.h>

// Test this:
// define signext i32 @main(i32 signext %argc, ptr %argv) {
//     %sum = add i32 %argc, 20
//     ret i32 %sum
// }

int main() {
    inr::TUnit unit("TIRTest.cpp");
    inr::TypeMap map;

    auto main_f = unit.createFunction(
        map.getFunc(map.getI32(), {map.getI32(), map.getPtr()}, false), "main",
        inr::Linkage::Global, inr::TypeExt::SignExt);

    auto argc_f = main_f->getArg(0);
    auto argv_f = main_f->getArg(1);

    argc_f->setExt(inr::TypeExt::SignExt);
    argc_f->setName("argc");

    argv_f->setName("argv");

    auto entry_f = unit.createBlock(map, main_f, "entry");

    auto sum_f = inr::AddInst::createAdd(
        entry_f, argc_f, unit.createConst(map.getI32(), inr::bigint(32, 42)),
        "sum");

    inr::RetInst::createRet(map, entry_f, sum_f);

    auto ret_fp = unit.createFunction(
        map.getFunc(map.getFloat(inr::FPFormat::Binary32),
                    {map.getFloat(inr::FPFormat::Binary32)}, false),
        "fp_ret", inr::Linkage::Global, inr::TypeExt::NoExt);
    ret_fp->getArg(0)->setName("fp_arg");

    auto ret_fp_entry = unit.createBlock(map, ret_fp, "entry");

    inr::RetInst::createRet(map, ret_fp_entry, ret_fp->getArg(0));

    if(!inr::Verifier::verify(unit, &inr::out())) {
        return 1;
    }

    inr::IRPrinter ir_printer(unit);
    ir_printer.print(inr::out());

    auto info = inr::TargetRegistry::getDesc(inr::host::getTarget());
    if(!info) {
        inr::out() << "Host target is not included\n";
        return 0;
    }
    inr::Translator ir_to_tir(info.get());
    auto tir_mod = ir_to_tir.translate(unit);

    inr::TIRPrinter tir_printer(tir_mod, info.get());
    tir_printer.print(inr::out());

    return 0;
}
