// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/IR/BlockDef.h>
#include <inr/Support/Stream.h>
#include <inr/TIR/Printer.h>
#include <inr/TIR/TBlock.h>
#include <inr/TIR/TIRT.h>
#include <inr/TIR/TInst.h>
#include <inr/TIR/TOperand.h>
#include <inr/TIR/TSymbol.h>
#include <inr/TIR/Translator.h>
#include <inr/Target/TargetDesc.h>

namespace inr {

void TIRPrinter::printType(stream& os, TIRT t) {
    switch(t.getKind()) {
        case TIRT::Bit:
            os << "b" << t.getWidth();
            break;
        case TIRT::Other:
            os << "other";
            break;
    }
}

static inline void printInstType(stream& os, TInst::InstType it) {
    switch(it) {
        case TInst::gRet:
            os << "gret";
            break;
        case TInst::gJmp:
            os << "gjmp";
            break;
        case TInst::gCmp:
            os << "gcmp";
            break;
        case TInst::gPhi:
            os << "gphi";
            break;
        case TInst::gAdd:
            os << "gadd";
            break;
        case TInst::gSub:
            os << "gsub";
            break;
        case TInst::gLoad:
            os << "gload";
            break;
        case TInst::gStore:
            os << "gstore";
            break;
        case TInst::gAlloca:
            os << "galloca";
            break;
        case TInst::gCopy:
            os << "gcopy";
            break;
        case TInst::gMul:
            os << "gmul";
            break;
        case TInst::gUDiv:
            os << "gudiv";
            break;
        case TInst::gSDiv:
            os << "gsdiv";
            break;
        case TInst::gURem:
            os << "gurem";
            break;
        case TInst::gSRem:
            os << "gsrem";
            break;
        case TInst::gShl:
            os << "gshl";
            break;
        case TInst::gLShr:
            os << "glshr";
            break;
        case TInst::gAShr:
            os << "gashr";
            break;
        case TInst::gAnd:
            os << "gand";
            break;
        case TInst::gOr:
            os << "gor";
            break;
        case TInst::gXor:
            os << "gxor";
            break;
        case TInst::GENERIC_LAST:
            break;
        case TInst::gInteger:
            os << "ginteger";
            break;
        case TInst::gUndef:
            os << "gundef";
            break;
    }
}

static inline void printTOperand(stream& os, TOperand op,
                                 const TargetInfo* tinfo) {
    switch(op.getKind()) {
        case TOperand::Register:
            os << '$' << tinfo->getRegisters()->registers[op.getRegN()].name;
            break;
        case TOperand::VRegister:
            os << '%' << op.getRegN();
            break;
        case TOperand::Immediate:
            os << op.getImm();
            break;
        case TOperand::BImmediate:
            os << *op.getBImm();
            break;
    }
}

void TIRPrinter::print(stream& os) const {
    os << "tir_module_name = " << mod_.getOriginal()->getName() << "\n\n";

    for(const TSymbol& symbol : mod_.getSyms()) {
        os << symbol.getOriginal()->getName() << ":\n";
        os.indent(2) << "args:\n";
        for(unsigned i = 0; i < symbol.getOriginal()->getNumArgs(); i++) {
            os.indent(4);
            auto arg = symbol.getOriginal()->getArg(i);

            printType(os, Translator::getType(arg->getType(), tinfo_));
            os << ' ';

            if(arg->getName().size()) {
                os << arg->getName();
            }
            else os << "<unnamed-arg-" << i << '>';
            os << '\n';
        }

        for(const TBlock& blk : symbol.getBlocks()) {
            os.indent(2) << blk.getOriginal()->getName() << ":\n";

            for(const TInst& inst : blk.getInstructions()) {
                os.indent(4);
                printInstType(os, inst.getInstType());
                os << ' ';
                printType(os, inst.getType());

                for(TOperand op : inst.getOperands()) {
                    os << ' ';
                    printTOperand(os, op, tinfo_);
                }

                os << '\n';
            }
        }
    }
}

} // namespace inr
