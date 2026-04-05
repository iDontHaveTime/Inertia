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
#include <inr/Target/Triple.h>

#include <cstdio>

int main(int argc, char** argv) {
    (inr::outs() << inr::reportInertiaVersion << '\n').flush();

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

    return 0;
}