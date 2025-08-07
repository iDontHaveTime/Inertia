#ifndef INERTIA_TARGETCODEGEN_HPP
#define INERTIA_TARGETCODEGEN_HPP

#include "Inertia/Target/TargetOutput.hpp"

namespace Inertia{
    struct TargetCodegen{
        const TargetOutput input;

        TargetCodegen() = delete;

        TargetCodegen(const TargetOutput& inp) noexcept : input(inp){};

        bool output();
    };
}

#endif // INERTIA_TARGETCODEGEN_HPP
