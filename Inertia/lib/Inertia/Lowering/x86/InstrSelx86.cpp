#include "Inertia/Lowering/x86/InstrSelx86.hpp"
#include "Inertia/IR/Block.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/Lowering/Lowered.hpp"
#include "Inertia/Mem/Arenalloc.hpp"

namespace Inertia{

bool InstructionSelectorx86::lower(Frame& frame, LoweredOutput& to){
    if(!to) return true;
    reserve_lower(frame, to);

    for(Function& func : frame.funcs){
        LoweredFunction& newFunc = to.funcs.emplace_back(&func, func.blocks.get_arena());
        for(ArenaReference<Block>& block : func.blocks){
            newFunc.blocks.emplace_back(block.get(), block->instructions.get_arena());
        }
    }

    return false;
}

}
