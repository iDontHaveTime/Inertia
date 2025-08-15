#ifndef INERTIA_FUNCTION_HPP
#define INERTIA_FUNCTION_HPP

#include "Inertia/Utils/Pow2Int.hpp"
#include <cstdint>
#include <string_view>

namespace Inertia{
    struct Function{
        std::string_view name;
        enum FunctionFlags : int32_t{
            LOCAL = 0x1,
            MANUAL_ALIGN = 0x2
        };
        int32_t flags = 0;
        Pow2Int align = 1;
    };
}

#endif // INERTIA_FUNCTION_HPP
