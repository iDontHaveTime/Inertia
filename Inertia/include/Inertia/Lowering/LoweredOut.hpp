#ifndef INERTIA_LOWEREDOUT_HPP
#define INERTIA_LOWEREDOUT_HPP

#include "Inertia/Debug/DebugInfo.hpp"
#include "Inertia/Lowering/Lowered.hpp"
#include "Inertia/Target/Triple.hpp"

namespace Inertia{
    struct LoweredOutput{
        DebugInfo* debug;
        TargetTriple* ttriple;
        std::vector<LoweredFunction> funcs;
    };
}

#endif // INERTIA_LOWEREDOUT_HPP
