#include "Inertia/Target/TargetManager.hpp"
#include "Inertia/Target/x86/Targetx86.int.hpp"
#include "Inertia/Target/AArch64/TargetAArch64.int.hpp"

namespace Inertia{

void TargetManager::load_target(TargetType target){
    if(currentTarget){
        close_target();
    }
    loaded = target;
    switch(target){
        case TargetType::None:
            return;
        case TargetType::x86:
            currentTarget = new InertiaTarget::Inertiax86::TargetBasex86();
            break;
        case TargetType::AArch64:
            currentTarget = new InertiaTarget::InertiaAArch64::TargetBaseAArch64();
            break;
    }
}

void TargetManager::close_target() noexcept{
    if(!currentTarget) return;
    delete currentTarget;
    currentTarget = nullptr;
    loaded = TargetType::None;
}

}