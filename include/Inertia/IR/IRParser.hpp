#ifndef INERTIA_IRPARSER_HPP
#define INERTIA_IRPARSER_HPP

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

        const LexerFile* get_file() const noexcept{
            return file;
        }

        ArenaAlloc& get_allocator() noexcept{
            return allocator;
        }

        const ArenaAlloc& get_allocator() const noexcept{
            return allocator;
        }

        ~IRParser() = default;
    };
}

#endif // INERTIA_IRPARSER_HPP
