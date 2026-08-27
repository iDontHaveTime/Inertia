// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/IR/ArgDef.h>
#include <inr/IR/BlockDef.h>
#include <inr/IR/ConstDef.h>
#include <inr/IR/Def.h>
#include <inr/IR/FuncDef.h>
#include <inr/IR/InstDef.h>
#include <inr/IR/Printer.h>
#include <inr/IR/TUnit.h>
#include <inr/IR/Type.h>
#include <inr/Support/Stream.h>
#include <inr/Support/Unreachable.h>

namespace inr {

static inline void printTypeExt(stream& os, TypeExt ext) {
    switch(ext) {
        case TypeExt::NoExt:
            break;
        case TypeExt::SignExt:
            os << "signext";
            break;
        case TypeExt::ZeroExt:
            os << "zeroext";
            break;
    }
}

stream& IRPrinter::printDef(stream& os, const Def* def, bool prefix) {
    switch(def->getDefType()) {
        case Def::ArgDefType:
        case Def::BlockDefType:
        case Def::InstDefType:
            if(prefix) os << '%';
            break;
        case Def::FuncDefType:
            if(prefix) os << '@';
            break;
        case Def::ConstDefType:
            return os << ((const ConstDef*)def)->getInteger();
        case Def::UnDefDefType:
            return os << "undef";
    }

    if(auto sv = def->getName(); !sv.empty()) {
        os << sv;
    }
    else {
        auto [v, e] = defNames_.try_emplace(def, counter_);
        if(e) counter_++;
        os << *v;
    }

    return os;
}

static inline void printType(stream& os, const Type* type) {
    switch(type->getID()) {
        case Type::Integer:
            os << 'i' << type->as<IntType>()->getWidth();
            break;
        case Type::Pointer:
            os << "ptr";
            break;
        case Type::Void:
            os << "void";
            break;
        case Type::Block:
            os << "block";
            break;
        case Type::Function: {
            const FuncType* ft = type->as<FuncType>();
            printType(os, ft->getReturn());
            os << '(';
            for(unsigned i = 0; i < ft->getNumArgs(); i++) {
                if(i) os << ", ";
                printType(os, ft->getArg(i));
            }
            os << ')';
        } break;
        case Type::Float:
            switch(type->as<FPType>()->getFormat()) {
                case FPFormat::Binary16:
                    os << "binary16";
                    break;
                case FPFormat::Binary32:
                    os << "binary32";
                    break;
                case FPFormat::Binary64:
                    os << "binary64";
                    break;
                case FPFormat::x87_80:
                    os << "x87_80";
                    break;
            }
            break;
        case Type::Array: {
            const ArrayType* at = type->as<ArrayType>();
            os << '[';
            printType(os, at->getElement());
            os << " x " << at->getSize() << ']';
        }
    }
}

void IRPrinter::printRetInst(stream& os, const RetInst& inst) {
    os << "ret ";
    printType(os, inst.getType());
    if(inst.isRetVoid()) {
        return;
    }
    os << ' ';
    printDef(os, inst.getRetVal());
}

void IRPrinter::printJmpInst(stream& os, const JmpInst& inst) {
    os << "jmp ";

    for(unsigned i = 0; i < inst.getUses().size(); i++) {
        if(i) os << ", ";
        const Def* use = inst.getUses()[i];
        printType(os, use->getType());
        os << ' ';
        printDef(os, use);
    }
}

void IRPrinter::printCmpInst(stream& os, const CmpInst& inst) {
    printDef(os, &inst) << " = ";
    printType(os, inst.getLhs()->getType());
    os << " cmp.";

    switch(inst.getCond()) {
        case CmpInst::Equal:
            os << "eq";
            break;
        case CmpInst::NotEqual:
            os << "neq";
            break;
        case CmpInst::UGreater:
            os << "ug";
            break;
        case CmpInst::UGreaterEqual:
            os << "uge";
            break;
        case CmpInst::ULess:
            os << "ul";
            break;
        case CmpInst::ULessEqual:
            os << "ule";
            break;
        case CmpInst::SGreater:
            os << "sg";
            break;
        case CmpInst::SGreaterEqual:
            os << "sge";
            break;
        case CmpInst::SLess:
            os << "sl";
            break;
        case CmpInst::SLessEqual:
            os << "sle";
            break;
    }

    os << '(';
    printDef(os, inst.getLhs()) << ", ";
    printDef(os, inst.getRhs());
    os << ')';
}

void IRPrinter::printBinaryInst(stream& os, const BinaryInst& inst) {
    printDef(os, &inst) << " = ";

    printType(os, inst.getType());
    os << ' ';

    switch(inst.getInstType()) {
        case InstDef::Add:
            os << "add";
            break;
        case InstDef::Sub:
            os << "sub";
            break;
        case InstDef::Mul:
            os << "mul";
            break;
        case InstDef::UDiv:
            os << "udiv";
            break;
        case InstDef::SDiv:
            os << "sdiv";
            break;
        case InstDef::URem:
            os << "urem";
            break;
        case InstDef::SRem:
            os << "srem";
            break;
        case InstDef::Shl:
            os << "shl";
            break;
        case InstDef::LShr:
            os << "lshr";
            break;
        case InstDef::AShr:
            os << "ashr";
            break;
        case InstDef::And:
            os << "and";
            break;
        case InstDef::Or:
            os << "or";
            break;
        case InstDef::Xor:
            os << "xor";
            break;
        default:
            inr_unreachable("This is checked beforehand");
    }

    os << '(';
    printDef(os, inst.getLhs()) << ", ";
    printDef(os, inst.getRhs());
    os << ')';
}

void IRPrinter::printPhi(stream& os, const PhiInst& inst) {
    printDef(os, &inst) << " = ";
    printType(os, inst.getType());
    os << " phi(";

    for(unsigned i = 0; i < inst.getUses().size(); i++) {
        auto inc = inst.getIncoming(i);
        if(i) os << ", ";
        os << '[';
        printDef(os, inc.first);
        os << ", ";
        printDef(os, inc.second);
        os << ']';
    }

    os << ')';
}

void IRPrinter::printLoad(stream& os, const LoadInst& inst) {
    printDef(os, &inst) << " = ";
    printType(os, inst.getType());
    os << " load(";
    printDef(os, inst.getFrom());
    os << ')';
}

void IRPrinter::printStore(stream& os, const StoreInst& inst) {
    os << "store ";

    printDef(os, inst.getTo()) << ", ";
    printType(os, inst.getFrom()->getType());
    os << ' ';
    printDef(os, inst.getFrom());
}

void IRPrinter::printAlloca(stream& os, const AllocaInst& inst) {
    printDef(os, &inst) << " = alloca(";
    printType(os, inst.getAllocaType());
    os << ", ";

    printType(os, inst.getCount()->getType());
    os << ' ';
    printDef(os, inst.getCount());
    os << ')';
}

void IRPrinter::printInstruction(stream& os, const InstDef& inst) {
    switch(inst.getInstType()) {
        case InstDef::Ret:
            printRetInst(os, (const RetInst&)inst);
            break;
        case InstDef::Jmp:
            printJmpInst(os, (const JmpInst&)inst);
            break;
        case InstDef::Phi:
            printPhi(os, (const PhiInst&)inst);
            break;
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
        case InstDef::Xor:
            printBinaryInst(os, (const BinaryInst&)inst);
            break;
        case InstDef::Cmp:
            printCmpInst(os, (const CmpInst&)inst);
            break;
        case InstDef::Unreachable:
            os << "unreachable";
            break;
        case InstDef::Load:
            printLoad(os, (const LoadInst&)inst);
            break;
        case InstDef::Store:
            printStore(os, (const StoreInst&)inst);
            break;
        case InstDef::Alloca:
            printAlloca(os, (const AllocaInst&)inst);
            break;
    }
}

void IRPrinter::printBlock(stream& os, const BlockDef& blk) {
    printDef(os, &blk, false) << ":\n";
    for(const InstDef& inst : blk.getInstructions()) {
        os.indent(4);
        printInstruction(os, inst);
        os << '\n';
    };
}

void IRPrinter::printFunction(stream& os, const FuncDef& fd) {
    printSignature(os, fd);
    if(!fd.getBlocks().empty()) {
        os << " {\n";

        for(const BlockDef& blk : fd.getBlocks()) {
            printBlock(os, blk);
        }

        os << '}';
    }
    os << '\n';
}

void IRPrinter::printSignature(stream& os, const FuncDef& fd) {
    os << "def fn ";

    const FuncType* ft = fd.getType()->as<FuncType>();

    if(fd.getRetExt() != TypeExt::NoExt && ft->getReturn()->isInteger()) {
        printTypeExt(os, fd.getRetExt());
        os << ' ';
    }
    printType(os, ft->getReturn());
    os << ' ';

    switch(fd.getLinkage()) {
        case Linkage::Global:
            break;
        case Linkage::Local:
            os << "local ";
            break;
        case Linkage::Weak:
            os << "weak ";
            break;
    }

    printDef(os, &fd);

    os << '(';

    for(unsigned i = 0; i < fd.getNumArgs(); i++) {
        if(i) os << ", ";
        const ArgDef* ad = fd.getArg(i);
        printType(os, ad->getType());

        if(ad->getExt() != TypeExt::NoExt && ad->getType()->isInteger()) {
            os << ' ';
            printTypeExt(os, ad->getExt());
        }

        os << ' ';
        printDef(os, ad);
    }

    os << ')';
}

void IRPrinter::print(stream& os) {
    counter_ = 0;
    defNames_.clear();
    if(!unit_.getName().empty()) {
        os << "module.name = " << unit_.getName() << "\n\n";
    }

    for(const FuncDef& fd : unit_.getFuncs()) {
        printFunction(os, fd);
        os << '\n';
    }
}

} // namespace inr
