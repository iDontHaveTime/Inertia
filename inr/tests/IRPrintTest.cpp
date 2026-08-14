// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/IR/ArgDef.h>
#include <inr/IR/FuncDef.h>
#include <inr/IR/InstDef.h>
#include <inr/IR/Linkage.h>
#include <inr/IR/Printer.h>
#include <inr/IR/TUnit.h>
#include <inr/IR/TypeMap.h>
#include <inr/IR/Verifier.h>
#include <inr/Math/BigInt.h>

int main() {
    inr::TUnit unit("IRPrintTest.cpp");
    inr::TypeMap tm;

    inr::FuncDef* printf_decl = unit.createFunction(
        tm.getFunc(tm.getI32(), {tm.getPtr()}, true), "printf",
        inr::Linkage::Global, inr::TypeExt::SignExt);

    inr::FuncDef* main_def = unit.createFunction(
        tm.getFunc(tm.getI32(), {tm.getI32(), tm.getPtr()}, false), "main",
        inr::Linkage::Global, inr::TypeExt::SignExt);

    main_def->getArg(0)->setName("argc");
    main_def->getArg(0)->setExt(inr::TypeExt::SignExt);

    main_def->getArg(1)->setName("argv");

    auto main_entry = unit.createBlock(tm, main_def, "entry");

    inr::RetInst::createRet(tm, main_entry, main_def->getArg(0));

    auto void_def = unit.createFunction(
        tm.getFunc(tm.getVoid(), {tm.getI32()}, false), "void_test",
        inr::Linkage::Local, inr::TypeExt::NoExt);
    auto void_arg = void_def->getArg(0);
    void_arg->setName("arg1");

    auto void_entry = unit.createBlock(tm, void_def, "entry");
    auto void_iftrue = unit.createBlock(tm, void_def, "iftrue");
    auto void_iffalse = unit.createBlock(tm, void_def, "iffalse");
    auto void_final = unit.createBlock(tm, void_def, "final");

    auto void_cmp = inr::CmpInst::createCmp(tm, void_entry, inr::CmpInst::Equal,
                                            void_arg, void_arg, "res");

    inr::JmpInst::createJmpCond(tm, void_entry, void_cmp, void_iftrue,
                                void_iffalse);

    auto void_sub = inr::SubInst::createSub(
        void_iftrue, void_arg,
        unit.createConst(tm.getI32(), inr::bigint(32, 42)), "x1");
    inr::JmpInst::createJmp(tm, void_iftrue, void_final);

    auto void_add = inr::AddInst::createAdd(
        void_iffalse, void_arg,
        unit.createConst(tm.getI32(), inr::bigint(32, 42)), "x2");
    inr::JmpInst::createJmp(tm, void_iffalse, void_final);

    auto void_phi = inr::PhiInst::createPhi(void_final, tm.getI32(), "x3");

    void_phi->addIncoming(void_sub, void_iftrue);
    void_phi->addIncoming(void_add, void_iffalse);

    inr::RetInst::createRet(tm, void_final, nullptr);

    if(!inr::Verifier::verify(unit, &inr::out())) {
        return 1;
    }

    inr::IRPrinter printer(unit);

    printer.print(inr::out());

    return 0;
}
