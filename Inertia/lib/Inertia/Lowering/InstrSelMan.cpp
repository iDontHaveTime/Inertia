#include "Inertia/Lowering/InstrSelMan.hpp"
#include "Inertia/Lowering/AArch64/InstrSelAArch64.hpp"
#include "Inertia/Lowering/Generic/InstrSelGen.hpp"
#include "Inertia/Lowering/x86/InstrSelx86.hpp"
#include "Inertia/Target/AArch64/TargetAArch64.int.hpp"
#include "Inertia/Target/x86/Targetx86.int.hpp"

namespace Inertia{

bool InstructionSelectorGeneric::lower(Frame&, LoweredOutput&, InertiaTarget::TargetBase*){
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
            target_base = new InertiaTarget::Inertiax86::TargetBasex86();
            break;
        case TargetType::AArch64:
            internal = new InstructionSelectorAArch64();
            target_base = new InertiaTarget::InertiaAArch64::TargetBaseAArch64();
            break;
    }
}

}
