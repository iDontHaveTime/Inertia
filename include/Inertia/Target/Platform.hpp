#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include "Inertia/Target/ABI.hpp"
#include "Inertia/Target/Architecture.hpp"
#include "Inertia/Target/Register.hpp"
#include <cstdint>

namespace Inertia{
    class Platform{
    public:
        Architecture arch;
        ABI abi;
        const char* name;

        inline void AddRegister(Register reg){
            arch.registers.push_back(reg);
        }

        inline void AddRegister(std::string name, uint16_t size){
            arch.registers.emplace_back(name, size);
        }

        inline void SetABIField(Register*& field, Register& reg){
            field = &reg;
        }

        inline Register* FindRegister(const char* name, uint16_t size){
            for(Register& reg : arch.registers){
                if(reg.name == name && reg.size == size) return &reg;
            }
            return nullptr;
        }

        Platform() : 
            arch(), abi(), name(nullptr){};

        Platform(const char* _name, Architecture _arch, ABI _abi) :
            arch(_arch), abi(_abi), name(_name){};
    };
};

#endif // PLATFORM_HPP
