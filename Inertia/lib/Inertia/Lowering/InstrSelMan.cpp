#include "Inertia/Lowering/InstrSelMan.hpp"
#include "Inertia/Lowering/AArch64/InstrSelAArch64.hpp"
#include "Inertia/Lowering/Generic/InstrSelGen.hpp"
#include "Inertia/Lowering/x86/InstrSelx86.hpp"

namespace Inertia{

bool InstructionSelectorGeneric::lower(Frame& frame, LoweredOutput& to){
    if(frame.funcs.empty()) return false; // silence the warnings
    if(to.funcs.empty()) return false;
    return false;
}

void InstructionSelectorManager::load_target(TargetType _tt){
    if(internal){
        close_target();
    }
    tt = _tt;
    switch(_tt){
        case TargetType::None:
            return;
        case TargetType::x86:
            internal = new InstructionSelectorx86();
            break;
        case TargetType::AArch64:
            internal = new InstructionSelectorAArch64();
            break;
    }
}

}