#include "Inertia/IR/Function.hpp"
#include "Inertia/IR/IRParser.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/Lexer.hpp"
#include "Inertia/IR/IRKeywords.hpp"
#include "Inertia/Lexer/LexerOutput.hpp"
#include "Inertia/Lexer/TokenType.hpp"
#include <cstddef>
#include <cstdio>

using namespace Inertia;

LexerOutput GetLexedFile(const LexerFile& file){
    Lexer lexr;
    lexr.line_comment = TokenType::SlashSlash;
    lexr.multiline_end = TokenType::StarSlash;
    lexr.multiline_start = TokenType::SlashStar;
    lexr.throw_eof = true;
    size_t assume = lexr.assume(file);

    KeywordMap kwd = CreateIRKeywordMap();
    
    lexr.SetKeywords(kwd);

    return lexr.lex_perf(file, assume);
}

int main(){
    LexerFile file = "examples/inertia.inr";

    LexerOutput out = GetLexedFile(file);

    TypeAllocator talloc;

    IRParser parser(&file);

    auto frame = parser.parse_tokens(out, talloc);

    for(Function& func : frame.functions){
        std::cout<<"funcd ";
        Type* t = func.return_type;
        unsigned int ptrl = 0;
        while(t->getKind() == Type::POINTER){
            ptrl++;
            t = ((PointerType*)t)->pointee;
        }
        switch(t->getKind()){
            case Type::INTEGER:
                std::cout<<'i'<<((IntegerType*)t)->width;
                break;
            case Type::FLOAT:
                std::cout<<'f'<<((FloatType*)t)->accuracy * 32;
                break;
            case Type::VOID:
                std::cout<<"void";
                break;
            default:
                break;
        }

        while(ptrl--){
            std::cout<<'*';
        }

        std::cout<<' '<<func.name;

        std::cout<<std::endl;
    }

    return 0;
}