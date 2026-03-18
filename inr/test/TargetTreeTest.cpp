// // Copyright (c) 2026 Inertia Project
// // Distributed under the Boost Software License, Version 1.0.
// // See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

// #include <inr/ADT/ArrView.h>
// #include <inr/IR/Context.h>
// #include <inr/ISel/TargetTree.h>

// /// @brief This file is for testing a simple target tree.
// ///
// /// This test contains no target instructions as this is just for testing.
// /// This test has an imaginary architecture called E-ISA.

// /// @brief Lets define an opcode enum for the example.
// ///
// /// As the `inr::OpcodeType` says this enum SHOULD use it as the type.
// enum class EISAOpcodes : inr::OpcodeType {
//     ADD32RR,
//     ADD32RM,
//     ADD64RR,
//     ADD64RM,
//     MOV32RR,
//     MOV32RM,
//     MOV64RR,
//     MOV64RM
// };

// /// @brief Follow it up with operands.
// ///
// /// As the `inr::OperandType` says this enum SHOULD use it as the type.
// enum class EISAOperands : inr::OperandType { Reg, Mem };

// /// @brief Same thing here just for
// enum class EISAInstructionTypes : inr::InstructionType { ADD, MOV };

// /// If this is not a reason for why make a DSL, I don't know what is.
// inr::TreeNodeInitializerObject add64rr(const inr::InrContext& ctx) {
//     return {"add64rr",
//             inr::InstructionType(EISAInstructionTypes::ADD),
//             {},
//             inr::OpcodeType(EISAOpcodes::ADD64RR)};
// }

// inr::TreeNodeInitializerObject add64rm(const inr::InrContext& ctx) {
//     return {"add64rm",
//             inr::InstructionType(EISAInstructionTypes::ADD),
//             {{{inr::OperandType(EISAOperands::Reg), ctx.getI64()},
//               {inr::OperandType(EISAOperands::Mem), ctx.getI64()}}},
//             inr::OpcodeType(EISAOpcodes::ADD64RM)};
// }

// inr::TreeNodeInitializerObject add32rr(const inr::InrContext& ctx) {
//     return {"add32rr",
//             inr::InstructionType(EISAInstructionTypes::ADD),
//             {{{inr::OperandType(EISAOperands::Reg), ctx.getI32()},
//               {inr::OperandType(EISAOperands::Reg), ctx.getI32()}}},
//             inr::OpcodeType(EISAOpcodes::ADD32RR)};
// }

// inr::TreeNodeInitializerObject add32rm(const inr::InrContext& ctx) {
//     return {"add32rm",
//             inr::InstructionType(EISAInstructionTypes::ADD),
//             {{{inr::OperandType(EISAOperands::Reg), ctx.getI32()},
//               {inr::OperandType(EISAOperands::Mem), ctx.getI32()}}},
//             inr::OpcodeType(EISAOpcodes::ADD32RM)};
// }

// inr::TreeNodeInitializerObject mov64rr(const inr::InrContext& ctx) {
//     return {"mov64rr",
//             inr::InstructionType(EISAInstructionTypes::MOV),
//             {{{inr::OperandType(EISAOperands::Reg), ctx.getI64()},
//               {inr::OperandType(EISAOperands::Reg), ctx.getI64()}}},
//             inr::OpcodeType(EISAOpcodes::MOV64RR)};
// }

// inr::TreeNodeInitializerObject mov64rm(const inr::InrContext& ctx) {
//     return {"mov64rm",
//             inr::InstructionType(EISAInstructionTypes::MOV),
//             {{{inr::OperandType(EISAOperands::Reg), ctx.getI64()},
//               {inr::OperandType(EISAOperands::Mem), ctx.getI64()}}},
//             inr::OpcodeType(EISAOpcodes::MOV64RM)};
// }

// inr::TreeNodeInitializerObject mov32rr(const inr::InrContext& ctx) {
//     return {"mov32rr",
//             inr::InstructionType(EISAInstructionTypes::MOV),
//             {{{inr::OperandType(EISAOperands::Reg), ctx.getI32()},
//               {inr::OperandType(EISAOperands::Reg), ctx.getI32()}}},
//             inr::OpcodeType(EISAOpcodes::MOV32RR)};
// }

// inr::TreeNodeInitializerObject mov32rm(const inr::InrContext& ctx) {
//     return {"mov32rm",
//             inr::InstructionType(EISAInstructionTypes::MOV),
//             {{{inr::OperandType(EISAOperands::Reg), ctx.getI32()},
//               {inr::OperandType(EISAOperands::Mem), ctx.getI32()}}},
//             inr::OpcodeType(EISAOpcodes::MOV32RM)};
// }

// inr::TreeNodeObjectFunc funcs[] = {add64rr, add64rm, add32rr, add32rm,
//                                    mov64rr, mov64rm, mov32rr, mov32rm};

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

int main() {
    return 0;
}