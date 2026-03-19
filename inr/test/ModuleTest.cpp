// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/IR/Block.h>
#include <inr/IR/Context.h>
#include <inr/IR/Function.h>
#include <inr/IR/Instruction.h>
#include <inr/IR/Module.h>
#include <inr/ISel/LinearISel.h>
#include <inr/PassManager.h>
#include <inr/Support/Stream.h>
#include <inr/Support/Version.h>
#include <inr/Target/Triple.h>

#include <cstdio>

int main() {
    (inr::outs() << inr::reportInertiaVersion << '\n').flush();

    inr::InrContext ctx;

    inr::Module* mod = ctx.newModule("main_module");

    inr::Function* func = mod->newFunction(
        "main", ctx.getFunction(ctx.getI32(),
                                {ctx.getI32(),
                                 ctx.getPointer(ctx.getPointer(ctx.getI8()))}));

    func->setArgName(0, "argc");
    func->setArgName(1, "argv");

    inr::standard_file_stream exampleStream(
        fopen("inr/example/module_example.inr", "w"), true, 0);

    inr::Block* entry = func->newBlock("entry");

    auto add = inr::BinaryInst::createAdd(ctx.getIntConstant(ctx.getI32(), 20),
                                          func->getArg(0), "sum", entry);

    inr::ReturnInst::create(add, entry);

    mod->print(exampleStream);

    inr::outs() << "Default triple: " << inr::Triple::getDefaultTriple()
                << '\n';

    inr::PassManager pm(inr::Triple::getDefaultTriple(), ctx, *mod);

    pm.setISel(inr::TripleLinearISel::get(pm.getTriple()));

    pm.runISel();

    return 0;
}