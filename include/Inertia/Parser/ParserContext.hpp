#ifndef PARSERCONTEXT_HPP
#define PARSERCONTEXT_HPP

#include "Inertia/Lexer/LexerOutput.hpp"

namespace Inertia{
    class ParserContext{
        LexerOutput* lxo = nullptr;
    public:
        LexerOutput& lexer(){
            return *lxo;
        }

        ParserContext() = default;

        ParserContext(ParserContext& rhs) = default;
        ParserContext& operator=(ParserContext& rhs) = default;

        ParserContext(ParserContext&& rhs){
            if(this != &rhs){
                lxo = rhs.lxo;
                rhs.lxo = nullptr;
            }
        }

        ParserContext& operator=(ParserContext&& rhs){
            if(this != &rhs){
                lxo = rhs.lxo;
                rhs.lxo = nullptr;
            }
            return *this;
        }

        ~ParserContext() = default;

        ParserContext(LexerOutput& lex) : lxo(&lex){};

    };
}

#endif // PARSERCONTEXT_HPP
