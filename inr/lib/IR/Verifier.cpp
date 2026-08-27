// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/IR/ArgDef.h>
#include <inr/IR/BlockDef.h>
#include <inr/IR/FuncDef.h>
#include <inr/IR/InstDef.h>
#include <inr/IR/Type.h>
#include <inr/IR/Verifier.h>
#include <inr/Support/Stream.h>

namespace inr {

template<typename... Args>
static inline void printError(inr::stream* os, Args&&... args) {
    if(os) {
        (((*os) << "verifier: ").changeColor(col::RED, true) << "error: ")
            .resetColor();
        (((*os) << args), ...);
        (*os) << '\n';
    }
}

static inline bool verifyInstruction(const FuncDef& fn, const InstDef& inst,
                                     inr::stream* os) {
    bool err = false;

    switch(inst.getInstType()) {
        case InstDef::Ret: {
            const RetInst& ret = (const RetInst&)inst;
            if(fn.getType()->isFunction() &&
               ((const FuncType*)fn.getType())->getReturn() != ret.getType()) {
                printError(os, "return type mismatch");
                err = true;
            }
            if(!ret.isRetVoid()) {
                if(ret.getUses().empty()) {
                    printError(os, "return returns a value but has no value");
                    err = true;
                }
                else {
                    if(ret.getRetVal()->getType() != ret.getType()) {
                        printError(os,
                                   "return value and return instruction have "
                                   "different types");
                        err = true;
                    }
                }
            }
            switch(ret.getType()->getID()) {
                case Type::Integer:
                case Type::Pointer:
                case Type::Void:
                case Type::Float:
                    break;
                case Type::Function:
                case Type::Block:
                case Type::Array:
                    printError(os,
                               "return tries to return a non returnable type");
                    err = true;
                    break;
            }
        } break;
        case InstDef::Jmp: {
            const JmpInst& jinst = (const JmpInst&)inst;
            if(jinst.getUses().size() == 1) {
                if(!jinst.getNonCondBlock()->getType()->isBlock()) {
                    printError(os,
                               "jmp (non conditional) must jump to a block");
                    err = true;
                }
            }
            else if(jinst.getUses().size() == 3) {
                if(!jinst.getCondition()->getType()->isInteger()) {
                    printError(
                        os,
                        "jmp (conditional) condition must be an integer (i1)");
                    err = true;
                }
                else {
                    if((jinst.getCondition()->getType()->as<IntType>())
                           ->getWidth() != 1) {
                        printError(os,
                                   "jmp (conditional) condition must be i1");
                        err = true;
                    }
                }
                if(!jinst.getIfTrue()->getType()->isBlock()) {
                    printError(
                        os, "jmp (conditional) iftrue operand must be a block");
                    err = true;
                }
                if(!jinst.getIfFalse()->getType()->isBlock()) {
                    printError(
                        os,
                        "jmp (conditional) iffalse operand must be a block");
                    err = true;
                }
            }
            else {
                printError(os, "jmp can only have 1 or 3 operands");
                err = true;
            }
        } break;
        case InstDef::Unreachable:
            break;
        case InstDef::Phi: {
            const PhiInst& phi = (const PhiInst&)inst;
            if(phi.getBlocks().size() != phi.getUses().size()) {
                printError(
                    os, "phi operands (values) and blocks must match in count");
                err = true;
            }
            else {
                for(unsigned i = 0; i < phi.getIncomingCount(); i++) {
                    auto inc = phi.getIncoming(i);
                    switch(inc.first->getType()->getID()) {
                        case Type::Integer:
                        case Type::Pointer:
                        case Type::Float:
                            break;
                        case Type::Void:
                        case Type::Block:
                        case Type::Function:
                        case Type::Array:
                            printError(os, "phi can only accept value types");
                            err = true;
                            break;
                    }
                    if(!inc.second->getType()->isBlock()) {
                        printError(os,
                                   "phi incoming block is not a block type");
                        err = true;
                    }
                }
            }
        } break;
        case InstDef::Cmp:
        case InstDef::Add:
        case InstDef::Sub:
        case InstDef::Mul:
        case InstDef::UDiv:
        case InstDef::SDiv:
        case InstDef::URem:
        case InstDef::SRem:
        case InstDef::Shl:
        case InstDef::LShr:
        case InstDef::AShr:
        case InstDef::And:
        case InstDef::Or:
        case InstDef::Xor: {
            const BinaryInst& binst = (const BinaryInst&)inst;
            if(binst.getUses().size() != 2) {
                printError(os, "binary instruction doesn't have 2 operands");
                err = true;
            }
            else {
                if(binst.getLhs()->getType() != binst.getRhs()->getType()) {
                    printError(
                        os, "binary instruction lhs type does not match rhs");
                    err = true;
                }
                else if(binst.getInstType() != InstDef::Cmp &&
                        binst.getType() != binst.getLhs()->getType()) {
                    printError(
                        os,
                        "binary instruction type does not match operand type");
                    err = true;
                }
            }
            switch(binst.getType()->getID()) {
                case Type::Integer:
                    if(binst.getInstType() == InstDef::Cmp &&
                       (binst.getType()->as<IntType>()->getWidth() != 1)) {
                        printError(os,
                                   "comparison instruction's width is not i1");
                        err = true;
                    }
                    break;
                case Type::Pointer:
                case Type::Void:
                case Type::Float:
                case Type::Function:
                case Type::Block:
                case Type::Array:
                    printError(os, "binary instruction has an invalid type");
                    err = true;
                    break;
            }
        } break;
        case InstDef::Load: {
            const LoadInst& linst = (const LoadInst&)inst;
            if(linst.getUses().size() != 1) {
                printError(os, "load instruction should only have 1 operand");
                err = true;
            }
            else {
                if(!linst.getFrom()->getType()->isPointer()) {
                    printError(os,
                               "load instruction should load from a pointer");
                    err = true;
                }
            }
            switch(linst.getType()->getID()) {
                case Type::Integer:
                case Type::Pointer:
                case Type::Float:
                    break;
                case Type::Void:
                case Type::Block:
                case Type::Function:
                case Type::Array:
                    printError(os, "load instruction can only load values");
                    err = true;
                    break;
            }
        } break;
        case InstDef::Store: {
            const StoreInst& sinst = (const StoreInst&)inst;
            if(sinst.getUses().size() != 2) {
                printError(os, "store should have 2 operands");
                err = true;
            }
            else {
                if(!sinst.getTo()->getType()->isPointer()) {
                    printError(os, "store's destination should be a pointer");
                    err = true;
                }
                switch(sinst.getFrom()->getType()->getID()) {
                    case Type::Integer:
                    case Type::Pointer:
                    case Type::Float:
                        break;
                    case Type::Void:
                    case Type::Block:
                    case Type::Function:
                    case Type::Array:
                        printError(os, "store can only store values");
                        err = true;
                        break;
                }
            }
        } break;
        case InstDef::Alloca: {
            const AllocaInst& ainst = (const AllocaInst&)inst;
            if(ainst.getUses().size() != 1) {
                printError(os, "alloca should only have 1 operand");
                err = true;
            }
            else {
                if(!ainst.getCount()->getType()->isInteger()) {
                    printError(os,
                               "alloca's count operand should be an integer");
                    err = true;
                }
                switch(ainst.getAllocaType()->getID()) {
                    case Type::Integer:
                    case Type::Pointer:
                    case Type::Float:
                    case Type::Array:
                        break;
                    case Type::Void:
                    case Type::Block:
                    case Type::Function:
                        printError(os, "alloca can only allocate values");
                        err = true;
                        break;
                }
            }
        } break;
    }

    return !err;
}

static inline bool verifyBlock(const FuncDef& fn, const BlockDef& blk,
                               inr::stream* os) {
    bool err = false;

    if(!blk.getType()->isBlock()) {
        printError(os, "block does not have a block type");
        err = true;
    }

    bool terminated = false;

    for(const InstDef& inst : blk.getInstructions()) {
        if(!verifyInstruction(fn, inst, os)) err = true;
        if(inst.isTerminator()) {
            terminated = true;
            if(&inst != blk.getInstructions().listTail()) {
                printError(os, "block terminator is not the last instruction");
                err = true;
            }
        }
    }

    if(!terminated) {
        printError(os, "block is not terminated");
        err = true;
    }

    return !err;
}

static inline bool verifyFunction(const FuncDef& fn, inr::stream* os) {
    bool err = false;
    std::string_view fName = fn.getName();

    if(fName.empty()) {
        printError(os, "function name is empty");
        err = true;
        fName = "unnamed";
    }

    const Type* t = fn.getType();
    if(!t->isFunction()) {
        printError(os, "function ", fName, "'s type is not a function type");
        err = true;
    }
    else {
        const FuncType* ft = t->as<FuncType>();
        switch(ft->getReturn()->getID()) {
            case Type::Integer:
            case Type::Pointer:
            case Type::Void:
            case Type::Float:
                break;
            case Type::Function:
                printError(os, "function ", fName,
                           "'s return type is a function type");
                err = true;
                break;
            case Type::Block:
                printError(os, "function ", fName,
                           "'s return type is a block type");
                err = true;
                break;
            case Type::Array:
                printError(os, "function ", fName,
                           "'s return type is an array type");
                err = true;
                break;
        }
        if(ft->getNumArgs() != fn.getNumArgs()) {
            printError(os, "function ", fName,
                       "'s type arg count doesn't match function arg count");
            err = true;
        }
        else {
            for(unsigned i = 0; i < ft->getNumArgs(); i++) {
                const ArgDef* ad = fn.getArg(i);
                const Type* at = ft->getArg(i);
                switch(at->getID()) {
                    case Type::Integer:
                    case Type::Pointer:
                    case Type::Void:
                    case Type::Float:
                        break;
                    case Type::Function:
                        printError(os, "function ", fName, "'s arg ", i,
                                   " is a function type");
                        err = true;
                        break;
                    case Type::Block:
                        printError(os, "function ", fName, "'s arg ", i,
                                   " is a block type");
                        err = true;
                        break;
                    case Type::Array:
                        printError(os, "function ", fName, "'s arg ", i,
                                   " is an array type");
                        err = true;
                        break;
                }
                if(ad->getType() != at) {
                    printError(os, "function ", fName, "'s arg ", i,
                               " type does not match the type arg");
                    err = true;
                }
            }
        }
    }

    for(const BlockDef& blk : fn.getBlocks()) {
        if(!verifyBlock(fn, blk, os)) err = true;
    }

    return !err;
}

bool Verifier::verify(const TUnit& unit, inr::stream* os) {
    bool err = false;

    for(const FuncDef& fn : unit.getFuncs()) {
        if(!verifyFunction(fn, os)) err = true;
    }

    return !err;
}

} // namespace inr
