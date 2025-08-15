#ifndef INERTIA_IRPARSER_HPP
#define INERTIA_IRPARSER_HPP

#include "Inertia/IR/IRBuilder.hpp"
#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/LexerOutput.hpp"

namespace Inertia{
    class IRParser{
        const LexerFile* file;
    public:

        IRParser() = delete;
        IRParser(const LexerFile* _file) noexcept : file(_file){};

        bool parse_tokens(const LexerOutput& tokens, IRBuilder& builder);

        const LexerFile* get_file() const noexcept{
            return file;
        }

        ~IRParser() = default;
    };
}

#endif // INERTIA_IRPARSER_HPP
