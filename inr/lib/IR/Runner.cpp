// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/IR/Argument.h>
#include <inr/IR/Block.h>
#include <inr/IR/Constant.h>
#include <inr/IR/Function.h>
#include <inr/IR/Instruction.h>
#include <inr/IR/Runner.h>
#include <inr/IR/Type.h>
#include <inr/IR/Value.h>
#include <inr/Support/Logger.h>
#include <inr/Target/Triple.h>

#include <initializer_list>
#include <unordered_map>

namespace inr {

using replc_t = std::unordered_map<const Value*, const Value*>;

static inline void replaceOperand(replc_t& replacements, const Value*& op) {
    switch(op->getValueID()) {
        case Value::ValueID::Argument:
        case Value::ValueID::Instruction:
            op = replacements[op];
            break;
        case Value::ValueID::ConstantInt:
        case Value::ValueID::Function:
        case Value::ValueID::Block:
            break;
    }
}

static inline void executeBinaryOp(const InrContext& ctx, replc_t& replacements,
                                   const BinaryInst* inst) {
    const Value* lhs = inst->getOperand(0);
    const Value* rhs = inst->getOperand(1);

    replaceOperand(replacements, lhs);
    replaceOperand(replacements, rhs);

    replacements[inst] =
        ctx.getIntConstant((const IntegerType*)lhs->getType(),
                           ((const ConstantInt*)lhs)->getValue() +
                               ((const ConstantInt*)rhs)->getValue());
}

static inline void executeInstruction(const InrContext& ctx,
                                      replc_t& replacements,
                                      const Instruction* inst) {
    switch(inst->getID()) {
        case Instruction::InstructionID::ADD:
            executeBinaryOp(ctx, replacements, (const BinaryInst*)inst);
            break;
        default:
            break;
    }
}

static inline const Value* executeTypedFunction(
    const Function* func, const InrContext& ctx,
    std::initializer_list<const Value*> args) {
    if(args.size() != func->getType()->getArgs().size()) {
        log::sendargs(errs(), log::Level::ERROR, "inr-run",
                      "mismatched amount of args in function ",
                      func->getName());
        return nullptr;
    }

    replc_t replacements;

    for(unsigned i = 0; i < func->getArgs().size(); i++) {
        replacements[func->getArgs().data() + i] = args.begin()[i];
    }

    const Block* currentBlock = func->getBlocks().front();
    const Instruction* currentInstruction =
        currentBlock->getInstructions().front();

    while(true) {
        if(currentInstruction->isTerminator()) {
            if(currentInstruction->getID() ==
               Instruction::InstructionID::RETURN) {
                return replacements[currentInstruction->getOperand(0)];
            }
            currentBlock = currentBlock->getNext();
            currentInstruction = currentBlock->getInstructions().front();
        }
        else {
            executeInstruction(ctx, replacements, currentInstruction);
            currentInstruction = currentInstruction->getNext();
        }
    }
}

const ConstantInt* ModuleRunner::execute(const Module* mod,
                                         const InrContext& ctx, Triple target,
                                         int argc, char** argv) {
    // We assume that the caller ran verifying pass.
    const Type* expectedSignature =
        ctx.getFunction(ctx.getI32(), {ctx.getI32(), ctx.getPointer()});

    const Function* foundMain = nullptr;

    for(const Function& func : mod->getFunctions()) {
        if(func.getName() == "main") {
            if(func.getType() != expectedSignature) {
                log::send(errs(), log::Level::ERROR, "inr-run",
                          "main's signature is not as expected");
                return nullptr;
            }
            foundMain = &func;
            break;
        }
    }

    if(!foundMain) {
        log::send(errs(), log::Level::ERROR, "inr-run",
                  "no main function found");
        return nullptr;
    }

    return (const ConstantInt*)executeTypedFunction(
        foundMain, ctx,
        {ctx.getIntConstant(ctx.getI32(), argc),
         ctx.getIntConstant(ctx.getInt(target.getPointerWidth()),
                            (uintptr_t)argv)});
}

}; // namespace inr