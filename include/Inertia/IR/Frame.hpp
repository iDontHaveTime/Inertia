#ifndef INERTIA_FRAME_HPP
#define INERTIA_FRAME_HPP

#include "Inertia/IR/Function.hpp"
#include <vector>

namespace Inertia{
    class Frame{
    public:
        std::vector<Function> functions;
    };
}

#endif // INERTIA_FRAME_HPP
