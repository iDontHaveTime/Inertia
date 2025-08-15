#ifndef INERTIA_POW2INT_HPP
#define INERTIA_POW2INT_HPP

#include <cstdint>
#include "Inertia/Mem/Archmem.hpp"

namespace Inertia{
    class Pow2Int{
        uint32_t val;
        CONSTEXPRCPP void convert(uint32_t v) noexcept{
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

        CONSTEXPRCPP Pow2Int() noexcept = default;

        CONSTEXPRCPP Pow2Int(uint32_t v) noexcept{
            convert(v);
        }

        CONSTEXPRCPP Pow2Int& operator=(uint32_t v) noexcept{
            convert(v);
            return *this;
        }

        CONSTEXPRCPP Pow2Int& operator=(const Pow2Int&) noexcept = default;

        CONSTEXPRCPP operator uint32_t() const noexcept{
            return val;
        }

        ~Pow2Int() noexcept = default;
    };
}

#endif // INERTIA_POW2INT_HPP
