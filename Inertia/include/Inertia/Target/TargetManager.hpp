#ifndef INERTIA_TARGETMANAGER_HPP
#define INERTIA_TARGETMANAGER_HPP

#include "Inertia/Target/TargetBase.hpp"
#include "Inertia/Target/Triple.hpp"

namespace Inertia{
    class TargetManager{    
        TargetType loaded = TargetType::None;
        InertiaTarget::TargetBase* currentTarget = nullptr;
    public:
        TargetManager() noexcept = default;
        TargetManager(TargetType tt){
            load_target(tt);
        }
        TargetManager(const TargetTriple& tt){
            load_target(tt.getLoadedType());
        }

        void load_target(TargetType target);

        inline void load_target(const TargetTriple& tt){
            load_target(tt.getLoadedType());
        }

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
