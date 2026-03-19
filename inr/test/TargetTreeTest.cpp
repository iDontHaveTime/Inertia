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
#include <inr/Target/Triple.h>

int main() {
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

    if(!add32rr){
        inr::outs() << "Instruction not found.\n";
    }
    else{
        inr::outs() << "Instruction found: " << add32rr->getName() << '\n';
    }

    return 0;
}

// int main() {
//     /// For types.
//     inr::InrContext ctx;

//     /// Finally insert it all into the tree.
//     inr::TreeNodeBuilder treeBuilder{
//         ctx, inr::arrview<inr::TreeNodeObjectFunc>{
//                  funcs, sizeof(funcs) / sizeof(inr::TreeNodeObjectFunc)}};

//     inr::TargetTree* tree = treeBuilder.buildTree();

//     tree ? (inr::outs() << *tree)
//          : (inr::outs() << "Making the tree was unsuccessful\n");

//     /// Now I want to add register to memory, well how do I do that?
//     /// Simple, use the walker like this:
//     const inr::LeafNode* add = inr::Walker::walk(
//         tree, inr::InstructionType(EISAInstructionTypes::ADD),
//         {{{inr::OperandType(EISAOperands::Reg), ctx.getI64()},
//           {inr::OperandType(EISAOperands::Mem), ctx.getI64()}}});

//     /// Lets check if it found it.
//     add ? (inr::outs() << "Add i64 Reg, Mem was found!\n")
//         : (inr::outs() << "Add instruction was not found.\n");

//     /// Last check is if the opcodes match or not.
//     inr::outs() << "Does the opcode match: "
//                 << (add->getOp() == inr::OpcodeType(EISAOpcodes::ADD64RM))
//                 << '\n';

//     return 0;
// }

// int main() {
//     return 0;
// }