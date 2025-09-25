#ifndef INERTIA_BLOCK_HPP
#define INERTIA_BLOCK_HPP

#include "Inertia/IR/Instruction.hpp"
#include "Inertia/Mem/Arenalist.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include <string_view>

/* Block Class. 
 * The Block class stores the instructions inside the block (obvious).
 * Each block can have only one terminating instruction (like ret or jump).
 * A block can get its label generated for things like jumps.
*/

namespace Inertia{
    /* The Block class, storing the instructions. */
    struct Block{
        /* Name of the block. */
        std::string_view name;
        /* Instructions in it. */
        ArenaLList<IRInstruction> instructions;
        /* Flags of the block. */
        uint32_t flags;
        /* All available block flags. */
        enum : uint32_t{
            GENERATE_LABEL = 0x1
        };

        /* The block's constructor. */
        Block(const std::string_view& _name, ArenaAlloc* _arena) noexcept : name(_name), instructions(_arena), flags(0){};
    };
}

#endif // INERTIA_BLOCK_HPP
