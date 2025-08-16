#ifndef INERTIA_TARGETMANAGER_HPP
#define INERTIA_TARGETMANAGER_HPP

#include "Inertia/Target/TargetBase.hpp"

namespace Inertia{
    enum class TargetType{
        None, x86, AArch64
    };
    class TargetManager{    
        TargetType loaded = TargetType::None;
        InertiaTarget::TargetBase* currentTarget = nullptr;
    public:
        TargetManager() noexcept = default;
        TargetManager(TargetType tt){
            load_target(tt);
        }

        void load_target(TargetType target);
        void close_target() noexcept;

        ~TargetManager() noexcept{
            close_target();
        }

        TargetType getLoadedType() const noexcept{
            return loaded;
        }

        InertiaTarget::TargetBase* getRaw() noexcept{
            return currentTarget;
        }
    };
}

#endif // INERTIA_TARGETMANAGER_HPP
