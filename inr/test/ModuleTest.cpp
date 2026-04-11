// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/IR/Block.h>
#include <inr/IR/Context.h>
#include <inr/IR/Function.h>
#include <inr/IR/Instruction.h>
#include <inr/IR/Module.h>
#include <inr/IR/Verifier.h>
#include <inr/MIR/MachineModule.h>
#include <inr/Support/Stream.h>
#include <inr/Support/Version.h>
#include <inr/TIR/StoreCleanup.h>
#include <inr/TIR/TIRISel.h>
#include <inr/TIR/TIRLowering.h>
#include <inr/TIR/TIRModule.h>
#include <inr/TIR/TIRPass.h>
#include <inr/Target/Flags.h>
#include <inr/Target/Triple.h>
#include <inr/Target/x86/x86AsmPrinter.h>

#include <cstdio>

int main(int argc, char** argv) {
    (inr::outs() << inr::reportInertiaVersion << '\n').flush();

    inr::Flags compFlags;

    if(argc > 1) {
        compFlags.setFrameRegister(inr::sview(argv[1]) == "frame");
    }

    inr::InrContext ctx;

    inr::Module* mod = ctx.newModule("main_module");

    inr::Function* func = mod->newFunction(
        "main",
        ctx.getFunction(ctx.getI32(), {ctx.getI32(), ctx.getPointer()}));

    func->setArgName(0, "argc");
    func->setArgName(1, "argv");

    inr::standard_file_stream exampleStream(
        fopen("inr/example/module_example.inr", "w"), true, 0);

    inr::Block* entry = func->newBlock(ctx, "entry");

    auto add = inr::BinaryInst::createAdd(
        func->getArg(0), ctx.getIntConstant(ctx.getI32(), 20), "sum", entry);

    inr::ReturnInst::create(add, entry);

    inr::ModuleErrors errs = inr::verifyModule(mod);
    if(errs.hasErrors()) {
        errs.printAll(inr::errs());
        return 1;
    }

    mod->print(exampleStream);

    inr::outs() << "Default triple: " << inr::Triple::getDefaultTriple()
                << '\n';

    inr::TIRLowering tir(inr::Triple::getDefaultTriple(), compFlags);

    auto tirmod = tir.lowerSSA(mod);
    inr::standard_file_stream tirexampleStream(
        fopen("inr/example/module_example.tir", "w"), true, 0);

    tir.print(tirmod.get(), tirexampleStream);

    inr::standard_file_stream tirregallocexampleStream(
        fopen("inr/example/module_example_regalloc.tir", "w"), true, 0);

    tir.linearRegallocPass(tirmod);

    inr::TIRPassManager pm(tirmod.get());
    inr::StoreCleanup storepass;
    pm.run(&storepass);

    tir.print(tirmod.get(), tirregallocexampleStream);

    inr::TIRISel isel(tir);
    auto mmod = isel.select(tirmod.get());

    inr::x86::x86AsmPrinter asmPrinter(tir.getTriple());

    inr::standard_file_stream asmexampleStream(
        fopen("inr/example/module_example.s", "w"), true, 0);
    asmPrinter.emit(asmexampleStream, mmod.get());

    return 0;
}