#include <inr/ADT/ArrView.h>
#include <inr/IR/Context.h>
#include <inr/ISel/TargetTree.h>

/// @brief This file is for testing a simple target tree.
///
/// This test contains no target instructions as this is just for testing.
/// This test has an imaginary architecture called E-ISA.

/// @brief Lets define an opcode enum for the example.
///
/// As the `inr::OpcodeType` says this enum SHOULD use it as the type.
enum class EISAOpcodes : inr::OpcodeType {
    ADD32RR,
    ADD32RM,
    ADD64RR,
    ADD64RM,
    MOV32RR,
    MOV32RM,
    MOV64RR,
    MOV64RM
};

/// @brief Follow it up with operands.
///
/// As the `inr::OperandType` says this enum SHOULD use it as the type.
enum class EISAOperands : inr::OperandType { RR, RM };

/// @brief Same thing here just for
enum class EISAInstructionTypes : inr::InstructionType { ADD, MOV };

/// If this is not a reason for why make a DSL, I don't know what is.
inr::TreeNodeInitializerObject add64rr(const inr::InrContext& ctx) {
    return {ctx.getI64(), "add64rr",
            inr::InstructionType(EISAInstructionTypes::ADD),
            inr::OperandType(EISAOperands::RR),
            inr::OpcodeType(EISAOpcodes::ADD64RR)};
}

inr::TreeNodeInitializerObject add64rm(const inr::InrContext& ctx) {
    return {ctx.getI64(), "add64rm",
            inr::InstructionType(EISAInstructionTypes::ADD),
            inr::OperandType(EISAOperands::RM),
            inr::OpcodeType(EISAOpcodes::ADD64RM)};
}

inr::TreeNodeInitializerObject add32rr(const inr::InrContext& ctx) {
    return {ctx.getI32(), "add32rr",
            inr::InstructionType(EISAInstructionTypes::ADD),
            inr::OperandType(EISAOperands::RR),
            inr::OpcodeType(EISAOpcodes::ADD32RR)};
}

inr::TreeNodeInitializerObject add32rm(const inr::InrContext& ctx) {
    return {ctx.getI32(), "add32rm",
            inr::InstructionType(EISAInstructionTypes::ADD),
            inr::OperandType(EISAOperands::RM),
            inr::OpcodeType(EISAOpcodes::ADD32RM)};
}

inr::TreeNodeInitializerObject mov64rr(const inr::InrContext& ctx) {
    return {ctx.getI64(), "mov64rr",
            inr::InstructionType(EISAInstructionTypes::MOV),
            inr::OperandType(EISAOperands::RR),
            inr::OpcodeType(EISAOpcodes::MOV64RR)};
}

inr::TreeNodeInitializerObject mov64rm(const inr::InrContext& ctx) {
    return {ctx.getI64(), "mov64rm",
            inr::InstructionType(EISAInstructionTypes::MOV),
            inr::OperandType(EISAOperands::RM),
            inr::OpcodeType(EISAOpcodes::MOV64RM)};
}

inr::TreeNodeInitializerObject mov32rr(const inr::InrContext& ctx) {
    return {ctx.getI32(), "mov32rr",
            inr::InstructionType(EISAInstructionTypes::MOV),
            inr::OperandType(EISAOperands::RR),
            inr::OpcodeType(EISAOpcodes::MOV32RR)};
}

inr::TreeNodeInitializerObject mov32rm(const inr::InrContext& ctx) {
    return {ctx.getI32(), "mov32rm",
            inr::InstructionType(EISAInstructionTypes::MOV),
            inr::OperandType(EISAOperands::RM),
            inr::OpcodeType(EISAOpcodes::MOV32RM)};
}

inr::TreeNodeObjectFunc funcs[] = {add64rr, add64rm, add32rr, add32rm,
                                   mov64rr, mov64rm, mov32rr, mov32rm};

int main() {
    /// For types.
    inr::InrContext ctx;

    /// Finally insert it all into the tree.
    inr::TreeNodeBuilder treeBuilder{
        ctx, inr::arrview<inr::TreeNodeObjectFunc>{
                 funcs, sizeof(funcs) / sizeof(inr::TreeNodeObjectFunc)}};

    inr::TargetTree* tree = treeBuilder.buildTree();

    tree ? (inr::outs() << *tree)
         : (inr::outs() << "Making the tree was unsuccessful\n");

    /// Now I want to add 2 32bit registers, well how do I do that?
    /// Simple, use the walker like this:
    const inr::LeafNode* add = inr::Walker::walk(
        tree, ctx.getI32(), inr::InstructionType(EISAInstructionTypes::ADD),
        inr::OperandType(EISAOperands::RR));

    /// Lets check if it found it.
    add ? (inr::outs() << "Add i32 Reg, Reg was found!\n")
        : (inr::outs() << "Add instruction was not found.\n");

    /// Last check is if the opcodes match or not.
    inr::outs() << "Does the opcode match: "
                << (add->getOp() == inr::OpcodeType(EISAOpcodes::ADD32RR))
                << '\n';

    return 0;
}