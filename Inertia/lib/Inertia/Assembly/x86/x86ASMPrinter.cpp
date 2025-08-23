#include "Inertia/Assembly/x86/x86ASMPrinter.hpp"
#include "Inertia/Assembly/Generic/ASMPrinterGen.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/Lowering/Lowered.hpp"
#include "Inertia/Target/Triple.hpp"
#include <fstream>
#include <ostream>
#include <sstream>

namespace Inertia{

enum class DebuggingPart{
    BEFORE, PROLOGUE, EPILOGUE, AFTER
};

void EmitFunctionEndLabel(const LoweredFunction& func, std::ostream& os){
    os<<".L_func_"<<func.original->name<<"_end";
}

bool EmitELFDebuggingFunction(const LoweredFunction& func, std::ostream& os, DebuggingPart dp){
    switch(dp){
        case DebuggingPart::BEFORE:
            os<<"\t.type "<<func.original->name<<",@function\n";
            break;
        case DebuggingPart::PROLOGUE:
            os<<"\t.cfi_startproc\n";
            break;
        case DebuggingPart::EPILOGUE:
            break;
        case DebuggingPart::AFTER:
            EmitFunctionEndLabel(func, os);
            os<<":\n";
            os<<"\t.size "<<func.original->name<<", ";
            EmitFunctionEndLabel(func, os);
            os<<'-'<<func.original->name<<'\n';
            os<<"\t.cfi_endproc\n";
            break;
    }

    return false;
}

bool EmitFunctionDebug(const LoweredOutput& lowout, const LoweredFunction& func, std::ostream& os, DebuggingPart dp){
    switch(lowout.ttriple->getFileType()){
        case FileType::None:
            return true;
        case FileType::ELF:
            return EmitELFDebuggingFunction(func, os, dp);
        case FileType::MachO:
            return false;
        case FileType::COFF:
            return false;
    }
    return true;
}

bool EmitFunctionEssentialsx86(const LoweredFunction& func, std::ostream& os){
    if(!func.original->check_flag(Function::LOCAL)){
        os<<"\t.globl "<<func.original->name<<'\n';
    }
    if(func.original->check_flag(Function::MANUAL_ALIGN)){
        os<<"\t.p2align "<<func.original->align.getPower()<<'\n';
    }
    else{
        os<<"\t.p2align 4\n";
    }
    return false;
}

bool PrintFunctionx86(const LoweredOutput& lowout, const LoweredFunction& func, std::ostream& os){
    EmitFunctionEssentialsx86(func, os);

    // before the function
    if(EmitFunctionDebug(lowout, func, os, DebuggingPart::BEFORE)){
        return true;
    }

    os<<func.original->name<<":\n";

    // prologue
    if(EmitFunctionDebug(lowout, func, os, DebuggingPart::PROLOGUE)){
        return true;
    }

    // BODY

    // epilogue on return

    // after the function
    if(EmitFunctionDebug(lowout, func, os, DebuggingPart::AFTER)){
        return true;
    }

    return false;
}

// MEMORIO is faster since it uses an in memory buffer then dumps it, but FILEIO is less ram consuming
bool x86ASMPrinter::output(const LoweredOutput& lowout, PrintingType pt){
    if(out.empty()) return true;
    if(!lowout) return true;
    if(lowout.ttriple->getLoadedType() != TargetType::x86){
        return true;
    }

    std::stringstream _ss_;
    std::ofstream _of_(out);
    if(!_of_) return true;

    std::ostream& os = pt == PrintingType::FILEIO ? (std::ostream&)_of_ : (std::ostream&)_ss_;

    for(const LoweredFunction& func : lowout.funcs){
        if(PrintFunctionx86(lowout, func, os)){
            return true;
        }
    }

    if(pt == PrintingType::MEMORYIO){
        _of_<<_ss_.rdbuf(); // dump the buffer to file
    }
    return false;
}

}
