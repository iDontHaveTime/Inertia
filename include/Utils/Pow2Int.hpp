#ifndef INERTIA_POW2INT_HPP
#define INERTIA_POW2INT_HPP

#include <cstdint>

namespace Inertia{
    class Pow2Int{
        uint32_t val;
        void convert(uint32_t v) noexcept{
            if(!v){
                val = 1;
                return;
            }
            if(!(v & (v - 1))){
                val = v;
                return;
            }
            v--;
            v |= v >> 1;
            v |= v >> 2;
            v |= v >> 4;
            v |= v >> 8;
            v |= v >> 16;
            v++;
            val = v;
        }
    public:

        Pow2Int() = default;
        Pow2Int(uint32_t v) noexcept{
            convert(v);
        }

        Pow2Int& operator=(uint32_t v) noexcept{
            convert(v);
            return *this;
        }
        Pow2Int& operator=(Pow2Int&) = default;

        operator uint32_t() noexcept{
            return val;
        }
        operator const uint32_t() const noexcept{
            return val;
        }

        ~Pow2Int() = default;
    };
}

#endif // INERTIA_POW2INT_HPP
