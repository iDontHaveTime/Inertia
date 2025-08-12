#ifndef INERTIA_FRAME_HPP
#define INERTIA_FRAME_HPP

#include "Inertia/IR/Function.hpp"
#include <vector>

namespace Inertia{
    struct Frame{
        std::vector<Function> funcs;
    };
}
    

#endif // INERTIA_FRAME_HPP
