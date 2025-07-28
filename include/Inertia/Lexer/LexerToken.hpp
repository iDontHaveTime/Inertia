#ifndef LEXERTOKEN_HPP
#define LEXERTOKEN_HPP

#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/TokenType.hpp"
#include "Inertia/Lexer/TokenBuild.hpp"
#include <string>

namespace Inertia{
    struct Token{
        size_t line;
        size_t start, end;
        std::string str; // for strings and character literals, doesnt store the token itself, most of the time not used
        uint32_t extraType = 0;
        TokenType type;

        Token() : line(0), start(0), end(0), type(TokenType::Special){};
        
        Token(Token& rhs) = default;
        Token& operator=(Token& rhs) = default;

        Token(Token&& rhs){
            if(this == &rhs) return;
            line = rhs.line;
            rhs.line = 0;
            start = rhs.start;
            rhs.start = 0;
            end = rhs.end;
            rhs.end = 0;
            str = std::move(rhs.str);
            extraType = rhs.extraType;
            rhs.extraType = 0;
            type = rhs.type;
            rhs.type = TokenType::Special;
        }

        Token& operator=(Token&& rhs){
            //(*this) = std::move(rhs);
            if(this == &rhs) return *this;
            line = rhs.line;
            rhs.line = 0;
            start = rhs.start;
            rhs.start = 0;
            end = rhs.end;
            rhs.end = 0;
            str = std::move(rhs.str);
            extraType = rhs.extraType;
            rhs.extraType = 0;
            type = rhs.type;
            rhs.type = TokenType::Special;
            return *this;
        }

        Token(size_t s, size_t e, TokenType t, size_t l) : line(l), start(s), end(e), type(t){};
        Token(size_t s, size_t e, TokenType t, size_t l, uint32_t ex) : line(l), start(s), end(e), extraType(ex), type(t){};
        Token(size_t s, size_t e, TokenType t, size_t l, TokenBuild& bld) : line(l), start(s), end(e), type(t){
            str = std::string(bld.data(), bld.index);
        };

        inline std::string view_str(const LexerFile& file) const{
            if(type == TokenType::StringLiteral || type == TokenType::CharLiteral) return str;
            return std::string(file.raw() + start, end - start);
        }

        inline std::string_view view(const LexerFile& file) const{
            if(type == TokenType::StringLiteral || type == TokenType::CharLiteral) return str;
            return std::string_view((file.raw() + start), end - start);
        }

        inline size_t len() const noexcept{
            return end - start;
        }

        operator bool() const noexcept{
            return end - start != 0;
        }
    };
    static Token dummyTok;
}

#endif // LEXERTOKEN_HPP
