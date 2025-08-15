#ifndef INERTIA_LEXERTOKEN_HPP
#define INERTIA_LEXERTOKEN_HPP

#include "Inertia/Lexer/TokenType.hpp"
#include "Inertia/Lexer/TokenBuild.hpp"
#include <string>

namespace Inertia{
    struct Token{
        size_t line;
        const char* start, *end;
        std::string str; // for strings and character literals, doesnt store the token itself, most of the time not used
        uint32_t extraType = 0;
        TokenType type;

        Token() noexcept : line(0), start(0), end(0), type(TokenType::Special){};
        
        Token(const Token& rhs) = default;
        Token& operator=(const Token& rhs) = default;

        Token(Token&& rhs) noexcept = default;
        Token& operator=(Token&& rhs) noexcept = default;

        Token(const char* s, const char*  e, TokenType t, size_t l) noexcept : line(l), start(s), end(e), type(t){};
        Token(const char*  s, const char*  e, TokenType t, size_t l, uint32_t ex) noexcept : line(l), start(s), end(e), extraType(ex), type(t){};
        Token(const char*  s, const char*  e, TokenType t, size_t l, TokenBuild& bld) : line(l), start(s), end(e), type(t){
            str = std::string(bld.data(), bld.index);
        };

        inline std::string view_str() const{
            if(type == TokenType::TokenEOF) return "EOF";
            if(type == TokenType::StringLiteral || type == TokenType::CharLiteral) return str;
            return std::string(start, end - start);
        }

        inline std::string_view view() const noexcept{
            if(type == TokenType::TokenEOF) return "EOF";
            if(type == TokenType::StringLiteral || type == TokenType::CharLiteral) return str;
            return std::string_view(start, end - start);
        }

        inline size_t len() const noexcept{
            return end - start;
        }

        inline int getKeyword() const noexcept{
            return type == TokenType::Keyword ? extraType : 0;
        }

        operator bool() const noexcept{
            return end - start != 0;
        }

        ~Token() noexcept = default;
    };
    static Token dummyTok;
}

#endif // INERTIA_LEXERTOKEN_HPP
