#ifndef INERTIA_CALLCONV_X86
#define INERTIA_CALLCONV_X86

#include "Inertia/Target/TargetBase.hpp"
#include "Inertia/Target/Triple.hpp"
#include "Inertia/Target/x86/Targetx86.int.hpp"

namespace InertiaTarget{
namespace Inertiax86{

    struct CallingConv{
        RegisterBase* returnReg;

        CallingConv() noexcept = default;

        void get_sysv(TargetBasex86* tb) noexcept{
            returnReg = tb->rax;
        }

        void get_cc(Inertia::ABIType abi, TargetBasex86* tb) noexcept{
            switch(abi){
                case Inertia::ABIType::None:
                    [[fallthrough]];
                case Inertia::ABIType::SystemV:
                    get_sysv(tb);
                    break;
            }
        }
    };

}
}

#endif // INERTIA_CALLCONV_X86
