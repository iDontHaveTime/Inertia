#ifndef INERTIA_ARCHMEM_HPP
#define INERTIA_ARCHMEM_HPP

#include <cstring>
#include <vector>
#include <cstdint>
#include <cstdio>

namespace Inertia{
    enum class Endian : uint8_t{
        IN_UNKNOWN_ENDIAN, IN_LITTLE_ENDIAN = 1, IN_BIG_ENDIAN = 2
    };
    #if __cplusplus >= 202002L
        #define CONSTEXPRCPP constexpr
    #else
        #define CONSTEXPRCPP
    #endif
    enum class Bitness : uint8_t{
        M_UNKNOWN_BIT, M_64_BIT, M_32_BIT 
    };
    constexpr Endian HardwareEndian(){
        switch(std::endian::native){
            case std::endian::little:
                return Endian::IN_LITTLE_ENDIAN;
            case std::endian::big:
                return Endian::IN_BIG_ENDIAN;
            default:
                return Endian::IN_UNKNOWN_ENDIAN;
        }
    }
    constexpr Endian MachineEndian = HardwareEndian();
    
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

#endif // INERTIA_ARCHMEM_HPP
