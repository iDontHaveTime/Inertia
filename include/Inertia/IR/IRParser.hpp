#ifndef INERTIA_IRPARSER_HPP
#define INERTIA_IRPARSER_HPP

#include "Inertia/IR/Function.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/IR/Frame.hpp"
#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/LexerOutput.hpp"
#include "Inertia/Mem/Arenalloc.hpp"

namespace Inertia{
    class IRParser{
        const LexerFile* file;
        ArenaAlloc allocator;
    public:

        IRParser() = delete;
        IRParser(const LexerFile* _file) noexcept : file(_file){};

        Frame parse_tokens(const LexerOutput& tokens, TypeAllocator& talloc);

        ~IRParser() = default;

        // todo: change std::string to lightweight appending specialized ones
        static std::string type_to_string(Type* t);
        static std::string function_to_string(Function& func);
        static std::string arg_to_string(Argument& arg);
    };
}

#endif // INERTIA_IRPARSER_HPP
