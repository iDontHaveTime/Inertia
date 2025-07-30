#ifndef INERTIA_TOKENSTREAM_HPP
#define INERTIA_TOKENSTREAM_HPP

#include "Inertia/Lexer/LexerOutput.hpp"
#include "Inertia/Lexer/LexerToken.hpp"
#include <cstddef>

namespace Inertia{
    class TokenStream{
        const LexerOutput* tokens;
        size_t index = 0;
    public:

        TokenStream() noexcept : tokens(nullptr){};
        TokenStream(const LexerOutput& toks) noexcept : tokens(&toks){};
        TokenStream(TokenStream&& rhs) noexcept{
            if(this != &rhs){
                tokens = rhs.tokens;
                index = rhs.index;
                rhs.tokens = nullptr;
                rhs.index = 0;
            }
        }

        ~TokenStream() noexcept = default;

        TokenStream& operator=(TokenStream&& rhs) noexcept{
            if(this != &rhs){
                tokens = rhs.tokens;
                index = rhs.index;
                rhs.tokens = nullptr;
                rhs.index = 0;
            }
            return *this;
        }

        TokenStream(const TokenStream&) noexcept = default;
        TokenStream& operator=(const TokenStream&) noexcept = default;

        const Token& current() const{
            if(!tokens){
                return dummyTok;
            }
            return (*tokens)[index];
        }

        TokenStream& operator<<(size_t i) noexcept{
            if(!tokens) return *this;
            if(index + i < tokens->size()){
                index += i;
            }
            return *this;
        }

        inline bool eof() const noexcept{
            return !tokens || index >= tokens->size();
        }

        inline bool peekable() const noexcept{
            return index + 1 < tokens->size();
        }

        inline bool peekablef() const noexcept{
            return peekable();
        }

        inline bool peekableb() const noexcept{
            return index != 0;
        }

        inline const Token& peek() const{
            if(!tokens) return dummyTok;
            return peekable() ? (*tokens)[index + 1] : tokens->tokens.back();
        }

        inline const Token& peekf() const{
            return peek();
        }

        inline const Token& peekb() const{
            if(!tokens) return dummyTok;
            return peekableb() ? (*tokens)[index - 1] : tokens->tokens.front();
        }

        inline size_t pos() const noexcept{
            return index;
        }

        TokenStream& operator>>(size_t i) noexcept{
            if(!tokens) return *this;
            if(i >= index){
                index = 0;
                return *this;
            }
            index -= i;
            return *this;
        }

        void reset() noexcept{
            index = 0;
        }

        inline void close() noexcept{
            tokens = nullptr;
            index = 0;
        }

        inline void destroy() noexcept{
            close();
        }
    };
}

#endif // INERTIA_TOKENSTREAM_HPP
