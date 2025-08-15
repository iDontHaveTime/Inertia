#ifndef INERTIA_LOWERED_HPP
#define INERTIA_LOWERED_HPP

#include "Inertia/IR/Function.hpp"

namespace Inertia{
    struct LoweredFunction{
        Function* original;
        
        LoweredFunction() noexcept = default;
        LoweredFunction(Function* func) noexcept : original(func){};
    };
}

#endif // INERTIA_LOWERED_HPP
