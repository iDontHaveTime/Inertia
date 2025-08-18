#ifndef INERTIA_LOWEREDOUT_HPP
#define INERTIA_LOWEREDOUT_HPP

#include "Inertia/Compiler/Flags.hpp"
#include "Inertia/Debug/DebugInfo.hpp"
#include "Inertia/Lowering/Lowered.hpp"
#include "Inertia/Target/Triple.hpp"

namespace Inertia{
    struct LoweredOutput{
        DebugInfo* debug;
        TargetTriple* ttriple;
        CompilerFlags* cflags;

        operator bool() const noexcept{
            return (debug && ttriple && cflags);
        }

        std::vector<LoweredFunction> funcs;
    };
}

#endif // INERTIA_LOWEREDOUT_HPP
