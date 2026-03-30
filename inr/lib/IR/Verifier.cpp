// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/IR/Argument.h>
#include <inr/IR/Block.h>
#include <inr/IR/Function.h>
#include <inr/IR/Instruction.h>
#include <inr/IR/Module.h>
#include <inr/IR/Type.h>
#include <inr/IR/Verifier.h>
#include <inr/Support/Logger.h>

#include <unordered_set>

namespace inr {

class ModuleVerifier {
    static void moduleVerify(ModuleErrors& errs, const Module* module);
    static void functionVerify(ModuleErrors& errs, const Function& func,
                               std::unordered_set<sview>& seen);
    static void blockVerify(ModuleErrors& errs, const Function& func,
                            const Block& block,
                            std::unordered_set<sview>& seen);
    static void instructionVerify(ModuleErrors& errs, const Function& func,
                                  const Instruction& inst);

public:
    static ModuleErrors verify(const Module* module);
};

ModuleErrors verifyModule(const Module* module) {
    return ModuleVerifier::verify(module);
}

ModuleErrors ModuleVerifier::verify(const Module* module) {
    ModuleErrors errs;

    moduleVerify(errs, module);

    return errs;
}

void ModuleVerifier::moduleVerify(ModuleErrors& errs, const Module* module) {
    std::unordered_set<sview> seen;

    for(const Function& func : module->getFunctions()) {
        functionVerify(errs, func, seen);
    }
}

void ModuleVerifier::functionVerify(ModuleErrors& errs, const Function& func,
                                    std::unordered_set<sview>& seen) {
    if(seen.contains(func.getName()))
        errs.addError(
            new FunctionError(&func, FunctionError::SubKind::Redefinition));
    else seen.emplace(func.getName());

    // Return type always matches.
    const FunctionType* signature = func.getType();

    if(signature->getArgs().size() != func.getArgs().size()) {
        errs.addError(new FunctionError(
            &func, FunctionError::SubKind::MismatchedSignature));
    }
    else {
        for(size_t i = 0; i < func.getArgs().size(); i++) {
            if(signature->getArgs()[i] != func.getArgs()[i].getType()) {
                errs.addError(new FunctionError(
                    &func, FunctionError::SubKind::MismatchedSignature));
                break;
            }
        }
    }

    if(func.getBlocks().front() == nullptr) {
        errs.addError(
            new FunctionError(&func, FunctionError::SubKind::NoEntryBlock));
    }

    std::unordered_set<sview> seenBlock;
    for(const Block& block : func.getBlocks()) {
        blockVerify(errs, func, block, seenBlock);
    }
}

void ModuleVerifier::blockVerify(ModuleErrors& errs, const Function& func,
                                 const Block& block,
                                 std::unordered_set<sview>& seen) {
    if(seen.contains(block.getName()))
        errs.addError(
            new BlockError(&block, BlockError::SubKind::Redefinition));
    else seen.emplace(block.getName());

    const Instruction* termInst = nullptr;
    bool errMultiTermSent = false;
    bool errNotLastSent = false;

    for(const Instruction& inst : block.getInstructions()) {
        if(termInst && !errNotLastSent) {
            errs.addError(new BlockError(
                &block, BlockError::SubKind::TerminatorIsntLast));
            errNotLastSent = true;
        }
        if(inst.isTerminator()) {
            if(!termInst) {
                termInst = &inst;
            }
            else {
                if(!errMultiTermSent) {
                    errs.addError(new BlockError(
                        &block, BlockError::SubKind::MultipleTerminators));
                    errMultiTermSent = true;
                }
            }
        }
        instructionVerify(errs, func, inst);
    }

    if(!termInst) {
        errs.addError(
            new BlockError(&block, BlockError::SubKind::NoTerminator));
    }
}

InstructionError* checkOperands(const Instruction& inst, const Type* expect) {
    for(const Value* val : inst.getOperands()) {
        if(val->getType() != expect)
            return new InstructionError(
                &inst, InstructionError::SubKind::OperandTypeMismatch);
    }
    return nullptr;
}

InstructionError* returnVerify(const Function& func, const ReturnInst& ret) {
    const Type* type = ret.getType();

    if(func.getType()->getReturn() != type) {
        return new InstructionError(
            &ret, InstructionError::SubKind::ReturnTypeMismatch);
    }

    if(type->isVoid()) {
        if(ret.getNumOperands())
            return new InstructionError(
                &ret, InstructionError::SubKind::TooManyOperands);
    }
    else {
        unsigned numOp = ret.getNumOperands();
        if(numOp > 1) {
            return new InstructionError(
                &ret, InstructionError::SubKind::TooManyOperands);
        }
        else if(numOp == 0) {
            return new InstructionError(
                &ret, InstructionError::SubKind::ReturnNoOperand);
        }
    }

    return checkOperands(ret, type);
}

InstructionError* binaryInstVerify(const BinaryInst& inst) {
    if(inst.getNumOperands() > 2) {
        return new InstructionError(&inst,
                                    InstructionError::SubKind::TooManyOperands);
    }
    else if(inst.getNumOperands() < 2) {
        return new InstructionError(
            &inst, InstructionError::SubKind::TooLittleOperands);
    }

    const Type* type = inst.getType();

    if(type->isVoid() || type->isFunction()) {
        return new InstructionError(&inst,
                                    InstructionError::SubKind::TypeNotAllowed);
    }

    return checkOperands(inst, type);
}

void ModuleVerifier::instructionVerify(ModuleErrors& errs, const Function& func,
                                       const Instruction& inst) {
    InstructionError* err = nullptr;
    switch(inst.getID()) {
        case Instruction::InstructionID::RETURN:
            err = returnVerify(func, (const ReturnInst&)inst);
            break;
        case Instruction::InstructionID::ADD:
            err = binaryInstVerify((const BinaryInst&)inst);
            break;
    }
    if(err) errs.addError(err);
}

/// @brief A helper function for errors.
template<typename... Args>
static inline void vError(raw_stream& os, Args&&... args) {
    log::sendargs(os, log::Level::ERROR, "inr-module-verifier",
                  std::forward<Args>(args)...);
}

void FunctionError::strerr(raw_stream& os) const {
    sview funcName = getFunction()->getName();
    switch(getSubKind()) {
        case SubKind::Redefinition:
            vError(os, "function ", funcName, " is already defined elsewhere");
            break;
        case SubKind::MismatchedSignature:
            vError(os, "function ", funcName, " has a mismatched signature");
            break;
        case SubKind::NoEntryBlock:
            vError(os, "function ", funcName, " has no blocks");
            break;
    }
}

void BlockError::strerr(raw_stream& os) const {
    sview blockName = getBlock()->getName();
    switch(getSubKind()) {
        case SubKind::Redefinition:
            vError(os, "block ", blockName, " is already defined elsewhere");
            break;
        case SubKind::NoTerminator:
            vError(os, "block ", blockName, " has no terminator");
            break;
        case SubKind::TerminatorIsntLast:
            vError(os, "block ", blockName,
                   " has its terminator as not the last instruction");
            break;
        case SubKind::MultipleTerminators:
            vError(os, "block ", blockName, " has more than one terminator");
            break;
    }
}

void InstructionError::strerr(raw_stream& os) const {
    sview instName =
        Instruction::getInstructionIDStr(getInstruction()->getID());
    switch(getSubKind()) {
        case SubKind::ReturnTypeMismatch:
            vError(os, instName, " has type '", *getInstruction()->getType(),
                   "' but the expected type was '",
                   *getInstruction()
                        ->getParent()
                        ->getParent()
                        ->getType()
                        ->getReturn(),
                   '\'');
            break;
        case SubKind::ReturnNoOperand:
            vError(os, instName, " has type '", *getInstruction()->getType(),
                   "' but has no return value");
            break;
        case SubKind::OperandTypeMismatch:
            vError(os, instName, " has operands with different types");
            break;
        case SubKind::TooManyOperands:
            vError(os, instName, " has too many operands");
            break;
        case SubKind::TooLittleOperands:
            vError(os, instName, " has too little operands");
            break;
        case SubKind::TypeNotAllowed:
            vError(os, instName,
                   " contains a disallowed type for that instruction");
            break;
    }
}

} // namespace inr