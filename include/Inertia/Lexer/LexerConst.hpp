#ifndef LEXERCONST_HPP
#define LEXERCONST_HPP

#include <cstdint>

namespace Inertia{
    #ifdef MAXTOKENLENGTH
    constexpr uint32_t MaxTokenLength = MAXTOKENLENGTH;
    #else
    constexpr uint32_t MaxTokenLength = 1024;
    #endif
}

#endif // LEXERCONST_HPP
