#ifndef INERTIA_FRAME_HPP
#define INERTIA_FRAME_HPP

#include "Inertia/IR/Function.hpp"
#include <vector>

namespace Inertia{
    class Frame{
    public:
        const char* filename;
        std::vector<Function> funcs;
    };
}

#endif // INERTIA_FRAME_HPP
