#ifndef INERTIA_FRAME_HPP
#define INERTIA_FRAME_HPP

#include "Inertia/IR/Function.hpp"
#include "Inertia/Target/Triple.hpp"
#include <vector>

namespace Inertia{
    struct Frame{
        TargetTriple* ttriple;
        std::vector<Function> funcs;

        Frame() noexcept = default;
        Frame(TargetTriple* ttrip) noexcept : ttriple(ttrip), funcs(){};
    };
}
    

#endif // INERTIA_FRAME_HPP
