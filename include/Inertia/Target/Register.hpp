#ifndef REGISTER_HPP
#define REGISTER_HPP

#include <cstdint>
#include <string>

namespace Inertia{
    // DO NOT FREE CHILDREN
    class Register{
    public:
        std::string name;
        // size of child MUST be below this
        Register* child;
        // size of parent MUST be above this
        Register* parent;
        
        uint32_t flags = 0; // COMPLETELY OPTIONAL AND MANAGED BY THE BACKEND IF NEEDED
        uint16_t size;

        Register() : name(), size(0){};

        Register(const char* _name, uint16_t _size) : name(_name), child(nullptr), parent(nullptr), size(_size){};
        Register(const char* _name, uint16_t _size, Register* _parent, Register* _child) : name(_name), size(_size){
            SetChild(_child);
            SetParent(_parent);
        };
        Register(std::string& _name, uint16_t _size) : name(_name), child(nullptr), parent(nullptr), size(_size){};
    
        ~Register() noexcept = default;

        inline void SetChild(Register* reg) noexcept{
            child = reg;
            if(!reg) return;
            reg->parent = this;
        }
        inline void SetParent(Register* reg) noexcept{
            parent = reg;
            if(!reg) return;
            reg->child = this;
        }

        inline void create(const std::string& str, Register* _parent, Register* _child){
            name = str;
            parent = _parent;
            if(_parent){
                _parent->child = this;
            }
            child = _child;
            if(_child){
                _child->parent = this;
            }
        }
    };
}

#endif // REGISTER_HPP
