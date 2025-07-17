#ifndef LEXER_HPP
#define LEXER_HPP

#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/LexerOutput.hpp"
#include "Inertia/Lexer/LexerToken.hpp"
#include "Inertia/Lexer/TokenType.hpp"
#include "Inertia/Lexer/TokenBuild.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <cstring>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Inertia{
    typedef std::unordered_map<std::string_view, uint32_t> KeywordMap;
    struct Lexer{
    private:
        TokenType lookup[256] = {TokenType::Special};
        const char* alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";
        const char* num = "0123456789";
        const char* symbols = "!@#$%^&*()_+-={}[]|\\?><,./\"':;~`";
        
        inline void ResetLookup() noexcept{
            for(size_t i = 0; i < 256; i++){
                lookup[i] = TokenType::Special;
            }
        }

        inline void InitLexer() noexcept{
            for(size_t i = 0; symbols[i] != '\0'; i++){
                lookup[(uint8_t)symbols[i]] = (TokenType)symbols[i];
            }
            for(size_t i = 0; alpha[i] != '\0'; i++){
                lookup[(uint8_t)alpha[i]] = TokenType::Alpha;
            }
            for(size_t i = 0; num[i] != '\0'; i++){
                lookup[(uint8_t)num[i]] = TokenType::Number;
            }
            /* explicit for safety */
            lookup[' '] = TokenType::Special;

            // continue
            for(uint16_t i = 0x80; i <= 0xBF; i++){
                lookup[i] = TokenType::UTF_Continue;
            }

            // 2 byte
            for(uint16_t i = 0xC0; i <= 0xDF; i++){
                lookup[i] = TokenType::UTF_2;
            }

            // 3 byte
            for(uint16_t i = 0xE0; i <= 0xEF; i++){
                lookup[i] = TokenType::UTF_3;
            }

            // 4 byte
            for(uint16_t i = 0xF0; i <= 0xF7; i++){
                lookup[i] = TokenType::UTF_4;
            }

        }
        bool customSymbol = false, replaceSymbol = false;
        TokenType (*customFunc)(char, char, char, size_t) = nullptr;
    public:
        KeywordMap keywords;
        std::string line_comment;
        std::string multiline_start, multiline_end;
        bool usekwd = false;
        
        Lexer() noexcept{
            InitLexer();
        }

        // put NULL or nullptr to skip
        Lexer(const char* _alpha, const char* _symbols, const char* _numbers){
            if(_alpha){
                alpha = _alpha;
            }
            if(_symbols){
                symbols = _symbols;
            }
            if(_numbers){
                num = _numbers;
            }
            ResetLookup();
            InitLexer();
        }

        void SetKeywords(KeywordMap& kwds){
            usekwd = true;
            keywords = kwds;
        }

        // few rules:
        /*
        Must return TokenType::Special if not found.
        Must follow size, if size is 2 only use lhs and mid, if 3 use all 3.

        Guarantees:
        Size is either 2 or 3.
        All chars are symbols.

        What is Replace:
        Means doesn't use the default symbol handling, you control it.
        */
        inline void AddSymbolHandling(TokenType (*func)(char lhs, char mid, char rhs, size_t size), bool replace = false) noexcept{
            if(!func) return;
            replaceSymbol = replace;
            customSymbol = true;
            customFunc = func;
        }
        
        inline bool isnum(uint8_t c) const noexcept{
            return lookup[c] == TokenType::Number;
        }
        inline bool isalpha(uint8_t c) const noexcept{
            return lookup[c] == TokenType::Alpha;
        }
        inline bool issymbol(uint8_t c) const noexcept{
            return (uint8_t)lookup[c] == c;
        }
        inline bool isspecial(uint8_t c) const noexcept{
            return lookup[c] == TokenType::Special;
        }

        TokenType getsymbol(uint8_t c) const noexcept{
            return lookup[c];
        }
        // pass in 'n' it will return '\n' and same for others
        static inline char escape(char c) noexcept{
            switch(c){
                case '\'':
                    return '\'';
                case '"':
                    return '"';
                case '\\':
                    return '\\';
                case '0':
                    return '\0';
                case 'a':
                    return '\a';
                case 'b':
                    return '\b';
                case 'e':
                    return '\e';
                case 'f':
                    return '\f';
                case 'n':
                    return '\n';
                case 'r':
                    return '\r';
                case 't':
                    return '\t';
                case 'v':
                    return '\v';
                default:
                    return '\0';
            }
        }

        inline TokenType look(uint8_t c){
            return lookup[c];
        }
        inline TokenType match(TokenBuild& tok) const noexcept{
            if(tok.index != 2 && tok.index != 3) return TokenType::Special;

            if(replaceSymbol && customSymbol && customFunc){
                return customFunc(tok.buffer[0], tok.buffer[1], tok.buffer[2], tok.index);
            }

            char lhs = tok.buffer[0];
            char mid = tok.buffer[1];
            if(tok.index == 2){
                switch(lhs){
                    case '=':
                        switch(mid){
                            case '=':
                                return TokenType::EqualsEquals;
                            default: break;
                        }
                        break;
                    case '&':
                        switch(mid){
                            case '&':
                                return TokenType::LogicalAnd;
                            case '=':
                                return TokenType::AmpersandEquals;
                            default: break;
                        }
                        break;
                    case '|':
                        switch(mid){
                            case '|':
                                return TokenType::LogicalOr;
                            case '=':
                                return TokenType::PipeEquals;
                            default: break;
                        }
                        break;
                    case '+':
                        switch(mid){
                            case '+':
                                return TokenType::PlusPlus;
                            case '=':
                                return TokenType::PlusEquals;
                            default: break;
                        }
                        break;
                    case '-':
                        switch(mid){
                            case '-':
                                return TokenType::MinusMinus;
                            case '=':
                                return TokenType::MinusEquals;
                            case '>':
                                return TokenType::Arrow;
                            default: break;
                        }
                        break;
                    case '*':
                        switch(mid){
                            case '=':
                                return TokenType::StarEquals;
                            case '/':
                                return TokenType::StarSlash;
                            default: break;
                        }
                        break;
                    case '/':
                        switch(mid){
                            case '=':
                                return TokenType::SlashEquals;
                            case '*':
                                return TokenType::SlashStar;
                            case '/':
                                return TokenType::SlashSlash;
                            default: break;
                        }
                        break;
                    case '%':
                        switch(mid){
                            case '=':
                                return TokenType::PercentEquals;
                            default: break;
                        }
                        break;
                    case '^':
                        switch(mid){
                            case '=':
                                return TokenType::CaretEquals;
                            default: break;
                        }
                        break;
                    case ':':
                        switch(mid){
                            case ':':
                                return TokenType::DoubleColon;
                            default: break;
                        }
                        break;
                    case '<':
                        switch(mid){
                            case '=':
                                return TokenType::LessEquals;
                            case '<':
                                return TokenType::LeftShift;
                            default: break;
                        }
                        break;
                    case '>':
                        switch(mid){
                            case '=':
                                return TokenType::GreaterEquals;
                            case '>':
                                return TokenType::RightShift;
                            default: break;
                        }
                        break;
                    default: break;
                }   
            }
            else{
                char rhs = tok.buffer[2];
                switch(lhs){
                    case '<':
                        switch(mid){
                            case '<':
                                switch(rhs){
                                    case '=':
                                        return TokenType::LeftShiftEquals;
                                    default: break;
                                }
                            default: break;
                        } 
                        break;
                    case '.':
                        switch(mid){
                            case '.':
                                switch(rhs){
                                    case '.':
                                        return TokenType::Ellipsis;
                                    default: break;
                                }
                                break;
                            default: break;
                        }
                        break;
                    case '>':
                        switch(mid){
                            case '>':
                                switch(rhs){
                                    case '=':
                                        return TokenType::RightShiftEquals;
                                    default: break;
                                }
                                break;
                            default: break;
                        }
                        break;
                    default: break;
                }
                // would be 3
                
            }
            if(customSymbol && customFunc){
                return customFunc(lhs, mid, tok.buffer[2], tok.index);
            }
            return TokenType::Special;
        }

        LexerOutput lex(LexerFile& file);
        size_t assume(const LexerFile& file);
        size_t find_split(const LexerFile& file);
        size_t find_split(const char* start, const char* end);
        LexerOutput lex_chunk(LexerFileChunk& chunk);
        LexerOutput lex_perf(LexerFile& file, size_t assumed);
        /* EXPERIMENTAL 
           THIS USES THREADING, USE AT YOUR OWN RISK!
           THIS IS THREAD SAFE BUT THIS ISNT TESTED MUCH.
        */
        LexerOutput split_lex(LexerFile* file);
        LexerOutput lex_2chunk(LexerFileChunk& chunk1, LexerFileChunk& chunk2, LexerFile* file);
        LexerOutput merge_output(LexerOutput&& out1, LexerOutput&& out2);
        std::vector<Token> merge(std::vector<Token>& toks1, std::vector<Token>& toks2);
    };
};

// USAGE EXAMPLE
/*
int LexerTest(){
    Inertia::LexerFile file("examples/assembly.s");

    if(!file){
        return 1;
    }

    Inertia::Lexer lexr;
    lexr.line_comment = "//";
    lexr.multiline_start = "/'*";
    lexr.multiline_end = "/'*"; colons inserted due to how C++ comments work
    
    Inertia::LexerOutput out = lexr.lex(file);
    return 0;
}

*/

#endif // LEXER_HPP
