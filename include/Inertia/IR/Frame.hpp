#ifndef INERTIA_FRAME_HPP
#define INERTIA_FRAME_HPP

#include "Inertia/IR/Function.hpp"
#include "Inertia/Lexer/LexerFile.hpp"
#include <vector>

namespace Inertia{
    class Frame{
    public:
        const LexerFile* file;
        std::vector<Function> funcs;
    };
}

#endif // INERTIA_FRAME_HPP
