#ifndef ASSEMBLY_HPP
#define ASSEMBLY_HPP

#include "Inertia/String/InrStr.hpp"
#include <string>

namespace Inertia{
    struct AssemblyInstruction{
        enum {REGISTER, LABEL, MEMORY, DIRECT} type;
        inrstr<16> mnemonic;
        std::string src1, src2, src3; // strings are kinda expensive
    };
}

#endif // ASSEMBLY_HPP
