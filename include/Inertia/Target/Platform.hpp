#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include "Inertia/Target/ABI.hpp"
#include "Inertia/Target/Architecture.hpp"
#include "Inertia/Target/Register.hpp"
#include <cstdint>
#include <string_view>

namespace Inertia{
    class Platform{
    public:
        Architecture* arch;
        ABI* abi;
        std::string_view name;

        std::string_view datasect;
        std::string_view rodatasect;
        std::string_view bsssect;

        inline void AddRegister(Register reg){
            arch->registers.push_back(reg);
        }

        inline void AddRegister(std::string name, uint16_t size){
            arch->registers.emplace_back(name, size);
        }

        inline void SetABIField(Register*& field, Register& reg){
            field = &reg;
        }

        inline const Register* FindRegister(const char* name, uint16_t size) const{
            for(const Register& reg : arch->registers){
                if(reg.name == name && reg.size == size) return &reg;
            }
            return nullptr;
        }

        Platform() : 
            arch(), abi(), name(){};

        Platform(const char* _name, Architecture* _arch, ABI* _abi) :
            arch(_arch), abi(_abi), name(_name){};
    };
};

#endif // PLATFORM_HPP
