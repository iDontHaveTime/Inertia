#ifndef INERTIA_LOWERED_HPP
#define INERTIA_LOWERED_HPP

#include "Inertia/IR/Block.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/Mem/Arenalist.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include "Inertia/Target/TargetBase.hpp"

namespace Inertia{
    struct LoweredInstruction{
        InertiaTarget::TargetInstruction* instruction;
    };
    struct LoweredBlock{
        const Block* original;
        ArenaLList<LoweredInstruction> instructions;

        LoweredBlock() noexcept = default;
        LoweredBlock(Block* block, ArenaAlloc* _arena) noexcept : original(block), instructions(_arena){};
    };
    struct LoweredFunction{
        const Function* original;
        ArenaLList<LoweredBlock> blocks;

        LoweredFunction() noexcept = default;
        LoweredFunction(Function* func, ArenaAlloc* _arena) noexcept : original(func), blocks(_arena){};
    };
}

#endif // INERTIA_LOWERED_HPP
