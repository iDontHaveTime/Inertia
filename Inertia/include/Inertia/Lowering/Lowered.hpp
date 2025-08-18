#ifndef INERTIA_LOWERED_HPP
#define INERTIA_LOWERED_HPP

#include "Inertia/IR/Block.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/Mem/Arenalist.hpp"
#include "Inertia/Mem/Arenalloc.hpp"

namespace Inertia{
    struct LoweredBlock{
        const Block* original;

        LoweredBlock() noexcept = default;
        LoweredBlock(const Block* block) noexcept : original(block){};
    };
    struct LoweredFunction{
        const Function* original;
        ArenaLList<LoweredBlock> blocks;
        
        LoweredFunction() noexcept = default;
        LoweredFunction(const Function* func, ArenaAlloc* _arena) noexcept : original(func), blocks(_arena){};
    };
}

#endif // INERTIA_LOWERED_HPP
