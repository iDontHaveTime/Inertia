#include "Inertia/Assembly/ASMPrinter.hpp"
#include "Inertia/Assembly/AArch64/AArch64ASMPrinter.hpp"
#include "Inertia/Assembly/Generic/ASMPrinterGen.hpp"
#include "Inertia/Assembly/x86/x86ASMPrinter.hpp"
#include "Inertia/Lowering/LoweredOut.hpp"
#include "Inertia/Target/TargetManager.hpp"

namespace Inertia{

bool ASMPrinterGeneric::output(const LoweredOutput& lowout, PrintingType pt){
    if(lowout.funcs.empty()) return true;
    if(pt == (PrintingType)-(int)pt) return false; // silence warnings
    return false;
}

void ASMPrinter::load_target(TargetType _tt){
    if(internal){
        close_target();
    }
    tt = _tt;
    switch(_tt){
        case TargetType::None:
            return;
        case TargetType::x86:
            internal = new x86ASMPrinter();
            break;
        case TargetType::AArch64:
            internal = new AArch64ASMPrinter();
            break;
    }
}

}