#include "Inertia/IR/IRParser.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/Lexer.hpp"
#include "Inertia/IR/IRKeywords.hpp"
#include "Inertia/Lexer/LexerOutput.hpp"
#include "Inertia/Lexer/TokenType.hpp"
#include <cstddef>
#include <cstdio>
#include <iostream>

using namespace Inertia;

LexerOutput GetLexedFile(const LexerFile& file){
    Lexer lexr;
    lexr.line_comment = TokenType::SlashSlash;
    lexr.multiline_end = TokenType::StarSlash;
    lexr.multiline_start = TokenType::SlashStar;
    size_t assume = lexr.assume(file);

    KeywordMap kwd = CreateIRKeywordMap();
    
    lexr.SetKeywords(kwd);

    return lexr.lex_perf(file, assume);
}
/*
void MakeELF(){
    ELF::ELFWriter wr("examples/inelf.o");

    wr.CreateHeader(
        ELF::ELF_Machine::X86_64, 
        ELF::ELF_ABI::SystemV, 
        ELF::ELF_Type::Relocatable, 
        Bitness::M_64_BIT, 
        Endian::IN_LITTLE_ENDIAN
    );

    wr.WriteHeader();
}
*/
int main(){
    LexerFile file = "examples/inertia.inr";

    LexerOutput out = GetLexedFile(file);

    TypeAllocator talloc;

    auto int32 = talloc.getInteger(32);

    std::cout<<(uint32_t)int32->getKind()<<' '<<int32->width<<' '<<int32<<std::endl;

    auto ptrint32 = talloc.getPointer(int32);
    auto ptr2int32 = talloc.getPointer(int32);

    std::cout<<(uint32_t)ptrint32->getKind()<<' '<<ptrint32->pointee<<' '<<ptrint32<<std::endl;
    std::cout<<(uint32_t)ptr2int32->getKind()<<' '<<ptr2int32->pointee<<' '<<ptr2int32<<std::endl;
    std::cout<<(ptrint32 == ptr2int32 ? "true" : "false")<<std::endl;

    IRParser parser(&file);

    //auto frame = parser.parse_tokens(out);

    return 0;
}