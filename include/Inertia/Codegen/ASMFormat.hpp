#ifndef ASMFORMAT_HPP
#define ASMFORMAT_HPP

#include "Inertia/Codegen/Assembly.hpp"
#include <vector>

namespace Inertia{
    class ASMFormat{
        std::vector<AssemblyVariant*> vars;
    public:
        inline void reserve(size_t c){
            vars.reserve(c);
        }
        inline void push(AssemblyVariant* op){
            vars.push_back(op);
        }
    };
}

#endif // ASMFORMAT_HPP
