#ifndef INERTIA_LOWERED_HPP
#define INERTIA_LOWERED_HPP

#include "Inertia/IR/Function.hpp"

namespace Inertia{
    struct LoweredFunction{
        const Function* original;
        
        LoweredFunction() noexcept = default;
        LoweredFunction(const Function* func) noexcept : original(func){};
    };
}

#endif // INERTIA_LOWERED_HPP
