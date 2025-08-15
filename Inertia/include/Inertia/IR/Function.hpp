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

        Function() = default;
        Function(const std::string_view& _name, int32_t _flags, uint32_t alignment = 1) noexcept : name(_name), flags(_flags), align(alignment){};
    };
}

#endif // INERTIA_FUNCTION_HPP
