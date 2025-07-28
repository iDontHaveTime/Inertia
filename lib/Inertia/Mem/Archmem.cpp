#include "Inertia/Mem/Archmem.hpp"
#include "Inertia/Target/Architecture.hpp"
#include <cstdint>

namespace Inertia{

const Endian MachineEndian = HardwareEndian();

Endian HardwareEndian(){
    uint32_t x = 1;
    char* byte_ptr = (char*)(&x);
    if(byte_ptr[0] == 1){
        return Endian::IN_LITTLE_ENDIAN;
    }
    return Endian::IN_BIG_ENDIAN;
}

void FlipMemEndian(void* mem, size_t size) noexcept{
    size_t lhs = 0, rhs = size - 1;

    uint8_t* mempt = (uint8_t*)mem;

    while(lhs < rhs){
        uint8_t temp = mempt[lhs];
        mempt[lhs] = mempt[rhs];
        mempt[rhs] = temp;
        lhs++;
        rhs--;
    }
}

void fread_endian(void* dest, size_t n, Endian endian, FILE* fst) noexcept{
    fread(dest, 1, n, fst);
    if(endian != MachineEndian){
        FlipMemEndian(dest, n);
    }
}

}