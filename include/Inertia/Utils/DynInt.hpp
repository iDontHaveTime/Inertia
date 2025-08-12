#ifndef INERTIA_DYNINT_HPP
#define INERTIA_DYNINT_HPP

#include <cstddef>
#include <cstdint>

namespace Inertia{
    template<size_t width>
    class DynInt{
        static_assert(width > 0, "Width cannot be 0");
        static constexpr size_t num_bytes = ((width + 7) & (~7)) / 8;
        uint8_t var[num_bytes] = {0};
    public:
        
        
    };
}

#endif // INERTIA_DYNINT_HPP
