#ifndef INERTIA_BLOCK_HPP
#define INERTIA_BLOCK_HPP

#include "Inertia/IR/Instruction.hpp"
#include "Inertia/Mem/Arenalist.hpp"
#include "Inertia/Mem/Arenalloc.hpp"

namespace Inertia{
    struct Block{
        std::string_view name;
        ArenaLList<IRInstruction> instructions;

        Block(const std::string_view& _name, ArenaAlloc* _arena) noexcept : name(_name), instructions(_arena){};
    };
}

#endif // INERTIA_BLOCK_HPP
