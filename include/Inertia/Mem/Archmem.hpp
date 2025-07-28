#ifndef ARCHMEM_HPP
#define ARCHMEM_HPP

#include <cstring>
#include <vector>
#include <cstdint>
#include <cstdio>

namespace Inertia{
    enum class Endian{
        IN_LITTLE_ENDIAN = 1, IN_BIG_ENDIAN = 2
    };
    enum class Bitness{
        M_64_BIT, M_32_BIT 
    };
    Endian HardwareEndian();

    extern const Endian MachineEndian;
    
    void FlipMemEndian(void* mem, size_t size) noexcept;

    template<typename T, typename Y>
    inline void InsertEndian(std::vector<T>& target, Y val, Endian endian){
        if(endian == MachineEndian){
            target.insert(target.end(), (uint8_t*)&val, (uint8_t*)&val + sizeof(Y));
            return;
        }

        uint8_t bytes[sizeof(Y)];
        memcpy(bytes, &val, sizeof(Y));
        FlipMemEndian(bytes, sizeof(Y));

        target.insert(target.end(), bytes, bytes + sizeof(Y));
    }

    void fread_endian(void* dest, size_t n, Endian endian, FILE* fst) noexcept;
}

#endif // ARCHMEM_HPP
