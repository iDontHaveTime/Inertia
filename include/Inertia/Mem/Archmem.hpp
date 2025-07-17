#ifndef ARCHMEM_HPP
#define ARCHMEM_HPP

#include "Inertia/Target/Architecture.hpp"
#include <cstring>

namespace Inertia{
    Endian HardwareEndian();

    extern const Endian MachineEndian;
    
    void FlipMemEndian(void* mem, size_t size);

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

    void fread_endian(void* dest, size_t n, Endian endian, FILE* fst);
}

#endif // ARCHMEM_HPP
