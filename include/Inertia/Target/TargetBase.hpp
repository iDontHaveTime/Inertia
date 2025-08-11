#ifndef INERTIA_TARGETBASE_HPP
#define INERTIA_TARGETBASE_HPP

#include "Inertia/Mem/Archmem.hpp"
#include <string_view>
#include <unordered_map>

namespace InertiaTarget{
    struct RegisterBase{
        std::string_view name;
        int classid;
        int width;
        RegisterBase* child = nullptr;
        RegisterBase* parent = nullptr;

        RegisterBase() = delete;

        inline void set_parent(RegisterBase* other) noexcept{
            parent = other;
            other->child = this;
        }

        inline void set_child(RegisterBase* other) noexcept{
            child = other;
            other->parent = this;
        }
        
        RegisterBase(const std::string_view& _name, int id, int w) noexcept : name(_name), classid(id), width(w){};
    };
    struct TargetBase{
        std::unordered_map<std::string_view, RegisterBase*> reg_database;
        Inertia::Endian endian;
        
        TargetBase() = delete;

        TargetBase(Inertia::Endian end) noexcept : endian(end){};

        virtual void init();
        virtual ~TargetBase();
    };
};

#endif // INERTIA_TARGETBASE_HPP
