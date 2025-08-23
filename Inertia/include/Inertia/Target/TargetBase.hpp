#ifndef INERTIA_TARGETBASE_HPP
#define INERTIA_TARGETBASE_HPP

#include "Inertia/Mem/Archmem.hpp"
#include <array>
#include <cstdint>
#include <string_view>
#include <unordered_map>
#include <ostream>

namespace InertiaTarget{
    enum class RegisterFlags : uint32_t{
        OCCUPIED
    };
    struct RegisterBase{
        RegisterBase* child = nullptr;
        RegisterBase* parent = nullptr;
        std::string_view name;
        int classid;
        int width;
        uint32_t flags;

        RegisterBase() = delete;

        inline void set_parent(RegisterBase* other) noexcept{
            parent = other;
            other->child = this;
        }

        inline void set_child(RegisterBase* other) noexcept{
            child = other;
            other->parent = this;
        }

        inline void set_flag(RegisterFlags flag) noexcept{
            flags |= (uint32_t)flag;
        }

        inline bool check_flag(RegisterFlags flag) const noexcept{
            return flags & (uint32_t)flag;
        }

        inline void clear_flag(RegisterFlags flag) noexcept{
            flags &= (uint32_t)flag;
        }

        inline const std::string_view& getName(void) const noexcept{
            return name;
        }

        RegisterBase(const std::string_view& _name, int id, int w) noexcept : name(_name), classid(id), width(w), flags(0){};

        virtual ~RegisterBase() noexcept = default;
    };
    struct TargetBase{
        std::unordered_map<std::string_view, RegisterBase*> reg_database;
        std::unordered_map<std::string_view, bool> extensions;
        Inertia::Endian endian;

        TargetBase() = delete;

        TargetBase(Inertia::Endian end) noexcept : endian(end){};

        virtual void init();
        virtual ~TargetBase() noexcept;
    };
    struct TargetInstructionResult{
        const RegisterBase* result;
        std::array<const RegisterBase*, 6> clobbers;
        size_t clobberSize;
    };
    struct TargetInstruction{
        TargetInstructionResult res;
        uint32_t id;

        virtual void emit(std::ostream& os);
        TargetInstruction(uint32_t _id, TargetInstructionResult _res) noexcept : res(_res), id(_id){};

        virtual ~TargetInstruction() noexcept = default;
    };
};

#endif // INERTIA_TARGETBASE_HPP
