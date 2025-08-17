#include "Inertia/Lowering/x86/InstrSelx86.hpp"
#include "Inertia/IR/Function.hpp"

namespace Inertia{

bool InstructionSelectorx86::lower(const Frame& frame, LoweredOutput& to){
    reserve_lower(frame, to);

    for(const Function& func : frame.funcs){
        to.funcs.emplace_back(&func);
    }

    return false;
}

}