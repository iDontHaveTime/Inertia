#ifndef LEXEROUTPUT_HPP
#define LEXEROUTPUT_HPP

#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/LexerToken.hpp"
#include <vector>

namespace Inertia{
    // start, end, type, line (constructor)
    struct LexerOutput{
        std::vector<Token> tokens;
        LexerFile* file;

        LexerOutput() : tokens(), file(nullptr){};

        LexerOutput(LexerFile* lfile) : tokens(), file(lfile){};
        LexerOutput(LexerFile& lfile) : tokens(), file(&lfile){};

        LexerOutput& operator=(LexerOutput& rhs) = default;
        LexerOutput(LexerOutput& rhs) = default;

        LexerOutput(LexerOutput&& rhs){
            if(this != &rhs){
                file = rhs.file;
                tokens = std::move(rhs.tokens);
                rhs.file = nullptr;
            }
        }

        LexerOutput& operator=(LexerOutput&& rhs){
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

        inline size_t TokensWithType(TokenType t){
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

        inline void push(size_t start, size_t end, TokenType type, size_t line){
            if(start == end){
                end++;
            }
            tokens.emplace_back(start, end, type, line);
        }

        inline void push(size_t start, size_t end, TokenType type, size_t line, uint32_t ex){
            if(start == end){
                end++;
            }
            tokens.emplace_back(start, end, type, line, ex);
        }

        inline void push(size_t start, size_t end, TokenType type, size_t line, TokenBuild& bld){
            if(start == end){
                end++;
            }
            tokens.emplace_back(start, end, type, line, bld);
        }

        auto begin(){
            return tokens.begin();
        }
        auto end(){
            return tokens.end();
        }
        auto begin() const{
            return tokens.begin();
        }
        auto end() const{
            return tokens.end();
        }
        auto cbegin() const{
            return tokens.cbegin();
        }
        auto cend() const{
            return tokens.cend();
        }
    };
}

#endif // LEXEROUTPUT_HPP
