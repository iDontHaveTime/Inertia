#ifndef ARCHITECTURE_HPP
#define ARCHITECTURE_HPP

#include "Inertia/Target/Register.hpp"
#include <cstdint>
#include <iostream>
#include <vector>

namespace Inertia{
    enum class Endian : uint8_t{
        IN_UNKNOWN_ENDIAN,
        IN_LITTLE_ENDIAN = 1,
        IN_BIG_ENDIAN = 2,
    };
    // not used in architecture class
    enum class Bitness : uint8_t{
        M_32_BIT = 1,
        M_64_BIT = 2,
    };
    class Architecture{
    public:
        std::string name;
        uint16_t pointerSize;
        Endian endian;

        std::vector<Register> registers;
        
        Architecture() = default;
        virtual ~Architecture() = default;

        Architecture(const char* _name, Endian _endian, uint32_t _pointerSize) :
            name(_name), pointerSize(_pointerSize), endian(_endian){};

        virtual void EnableExtension(const char* ext){
            // used for stuff like AVX, NEON, etc..
            std::cout<<ext<<" extension not found"<<std::endl;
        }
    };
}

/* x86-64 GAS SYNTAX */ /*
When external linking for PIC/PIE:
lets take .extern MyFunc
Calling:
call MyFunc@PLT

Getting function address (function pointers):
movq MyFunc@GOTPCREL(%rip), %rax
*/

/* ARCHITECTURE EXAMPLE x86-64 */ /*

class x8664Arch : public Architecture{
    bool avxenabled = false;
public:
    x8664Arch() : Architecture("x86-64", Endian::IN_LITTLE_ENDIAN, 8){
        registers.emplace_back("rax", 64);
        etc...
    };
    void EnableExtension(const char* ext) override{
        if(strcmp(ext, "avx") == 0 && !avxenabled){
            registers.emplace_back("ymm0", 256);
            etc...
            avxenabled = true;
            return;
        }
        std::cout<<ext<<" extension not found"<<std::endl;
    }
};


*/
    

#endif // ARCHITECTURE_HPP
