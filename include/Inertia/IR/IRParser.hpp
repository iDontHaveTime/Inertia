#ifndef INERTIA_IRPARSER_HPP
#define INERTIA_IRPARSER_HPP

#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/IR/Frame.hpp"
#include "Inertia/Lexer/LexerOutput.hpp"

namespace Inertia{
    class IRParser{
        const LexerFile* file = nullptr;
    public:

        IRParser() = delete;

        IRParser(const LexerFile* _file) noexcept : file(_file){};

        Frame parse_tokens(const LexerOutput& tokens);

        ~IRParser() = default;
    };
}

#endif // INERTIA_IRPARSER_HPP
