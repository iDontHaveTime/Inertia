#ifndef INERTIA_REGISTERALLOC_HPP
#define INERTIA_REGISTERALLOC_HPP

#include "Inertia/Target/TargetBase.hpp"

namespace Inertia{
    class RegisterAllocator{
    public:
        InertiaTarget::TargetBase* target;

        RegisterAllocator() = delete;
        RegisterAllocator(InertiaTarget::TargetBase* _target) noexcept : target(_target){};
        
        const std::unordered_map<std::string_view, InertiaTarget::RegisterBase*>& get_all_registers() const noexcept{
            return target->reg_database;
        }
    };
};

#endif // INERTIA_REGISTERALLOC_HPP
