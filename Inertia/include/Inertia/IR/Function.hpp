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

/* Function Class.
 * The Function class stores many stuff describing the function and what it should be, for example:
 * The name, arguments, flags (like is it manually aligned), linkage type, etc..
 * It also contains a list for Block classes, it has the SSA context list too.
*/

namespace Inertia{
    /* The Function class itself. */
    struct Function{
        /* Name of the function. */
        std::string_view name;
        /* Type of the function, aka the return value. */
        ArenaReference<Type> type;
        /* Arguments that the function takes. */
        ArenaLList<SSAArg> args;
        /* The Blocks inside the function. */
        ArenaLList<Block> blocks;
        /* The current unique SSA ID. */
        size_t ssaid = 0;
        /* Available function flags. */
        enum FunctionFlags : uint32_t{
            MANUAL_ALIGN = 0x1
        };
        /* The linkage type of the function. */
        enum class LinkageType{
            INTERNAL, EXTERNAL, WEAK
        } linkage; // The linkage variable.
        /* Flags that are applied to this function. */
        uint32_t flags = 0;
        /* Alignment of the function, if the alignment flag is on. */
        Pow2Int align = 1;

        /* The SSA context of this function. */
        std::unordered_map<size_t, SSACtx> ssactx;

        /* Checks if the function flag is set. */
        bool check_flag(FunctionFlags flag) const noexcept{
            return flags & flag;
        }

        /* No default constructor. */
        Function() = delete;
        /* The main function constructor. */
        Function(const std::string_view& _name, ArenaReference<Type> _type, ArenaAlloc* _arena, LinkageType _linkage, int32_t _flags, uint32_t alignment = 1) noexcept : name(_name), type(_type), args(_arena), blocks(_arena), linkage(_linkage), flags(_flags), align(alignment){};
    };
}

#endif // INERTIA_FUNCTION_HPP
