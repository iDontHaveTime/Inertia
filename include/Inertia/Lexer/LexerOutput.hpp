#ifndef INERTIA_LEXEROUTPUT_HPP
#define INERTIA_LEXEROUTPUT_HPP

#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/LexerToken.hpp"
#include <vector>

namespace Inertia{
    // start, end, type, line (constructor)
    struct LexerOutput{
        std::vector<Token> tokens;
        const LexerFile* file;

        LexerOutput() noexcept : tokens(), file(nullptr){};

        LexerOutput(const LexerFile* lfile) noexcept : tokens(), file(lfile){};
        LexerOutput(const LexerFile& lfile) noexcept : tokens(), file(&lfile){};

        LexerOutput& operator=(LexerOutput& rhs) = default;
        LexerOutput(LexerOutput& rhs) = default;

        LexerOutput(LexerOutput&& rhs) noexcept{
            if(this != &rhs){
                file = rhs.file;
                tokens = std::move(rhs.tokens);
                rhs.file = nullptr;
            }
        }

        LexerOutput& operator=(LexerOutput&& rhs) noexcept{
            if(this != &rhs){
                file = rhs.file;
                tokens = std::move(rhs.tokens);
                rhs.file = nullptr;
            }
            return *this;
        }

        ~LexerOutput() = default;

        inline size_t size() const noexcept{
            return tokens.size();
        }

        inline size_t TokensWithType(TokenType t) const noexcept{
            size_t res = 0;
            for(size_t i = 0; i < tokens.size(); i++) if(tokens[i].type == t) res++;
            return res;
        }

        const Token& operator[](size_t index) const{
            if(index >= tokens.size()){
                throw std::out_of_range("Out of range in LexerOutput tokens");
            }
            return tokens[index];
        }

        inline void push(const char* start, const char* end, TokenType type, size_t line){
            if(start == end){
                end++;
            }
            tokens.emplace_back(start, end, type, line);
        }

        inline void push(const char* start, const char* end, TokenType type, size_t line, uint32_t ex){
            if(start == end){
                end++;
            }
            tokens.emplace_back(start, end, type, line, ex);
        }

        inline void push(const char* start, const char* end, TokenType type, size_t line, TokenBuild& bld){
            if(start == end){
                end++;
            }
            tokens.emplace_back(start, end, type, line, bld);
        }

        auto begin() noexcept{
            return tokens.begin();
        }
        auto end() noexcept{
            return tokens.end();
        }
        auto begin() const noexcept{
            return tokens.begin();
        }
        auto end() const noexcept{
            return tokens.end();
        }
        auto cbegin() const noexcept{
            return tokens.cbegin();
        }
        auto cend() const noexcept{
            return tokens.cend();
        }
    };
}

#endif // INERTIA_LEXEROUTPUT_HPP
