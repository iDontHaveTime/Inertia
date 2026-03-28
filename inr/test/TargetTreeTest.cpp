// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

/// @file test/TargetTreeTest.cpp
/// @brief This file is for testing a simple target tree.
///
/// This test uses the x86 target tree for the example.

#include <inr/IR/Context.h>
#include <inr/ISel/TargetTree.h>
#include <inr/ISel/x86/TargetTree.h>
#include <inr/Support/Stream.h>
#include <inr/Support/Version.h>
#include <inr/Target/Triple.h>

int main() {
    (inr::outs() << inr::reportInertiaVersion << '\n').flush();

    // Needed for types
    inr::InrContext ctx;

    // Get the tree
    inr::TreeNodeBuilder x86Tree(
        ctx, inr::getTargetTreeInit(inr::Triple("x86_64-linux-gnu")));

    const inr::LeafNode* add32rr = inr::Walker::walk(
        x86Tree.buildTree(),
        inr::InstructionType(inr::x86::InstructionTypes::ADD),
        {{{inr::OperandType(inr::x86::Operands::Reg), ctx.getInt(32)},
          {inr::OperandType(inr::x86::Operands::Reg), ctx.getInt(32)}}});

    if(!add32rr) {
        inr::outs() << "Instruction not found.\n";
    }
    else {
        inr::outs() << "Instruction found: " << add32rr->getName() << '\n';
    }

    return 0;
}