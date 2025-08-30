#ifndef INERTIA_FUNCTION_HPP
#define INERTIA_FUNCTION_HPP

#include "Inertia/IR/Block.hpp"
#include "Inertia/IR/Instruction.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Mem/Arenalist.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include "Inertia/Utils/Pow2Int.hpp"
#include <cstdint>
#include <string_view>

namespace Inertia{
    struct Function{
        std::string_view name;
        ArenaReference<Type> type;
        ArenaLList<SSAValue> args;
        ArenaLList<Block> blocks;
        size_t ssaid = 0;
        enum FunctionFlags : int32_t{
            LOCAL = 0x1,
            MANUAL_ALIGN = 0x2
        };
        int32_t flags = 0;
        Pow2Int align = 1;

        std::unordered_map<size_t, SSACtx> ssactx;

        bool check_flag(FunctionFlags flag) const noexcept{
            return flags & flag;
        }

        Function() = delete;
        Function(const std::string_view& _name, ArenaReference<Type> _type, ArenaAlloc* _arena, int32_t _flags, uint32_t alignment = 1) noexcept : name(_name), type(_type), blocks(_arena), flags(_flags), align(alignment){};
    };
}

#endif // INERTIA_FUNCTION_HPP
