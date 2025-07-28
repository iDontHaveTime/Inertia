#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/Lexer.hpp"
#include "Inertia/IR/IRKeywords.hpp"
#include "Inertia/Lexer/LexerOutput.hpp"
#include "Inertia/Lexer/LexerToken.hpp"
#include "Inertia/Target/Architecture.hpp"
#include "Inertia/ELF/ELFWriter.hpp"
#include <cstddef>
#include <cstdio>

using namespace Inertia;

LexerOutput GetLexedFile(const LexerFile& file){
    Lexer lexr;
    lexr.line_comment = "//";
    lexr.multiline_end = "*/";
    lexr.multiline_start = "/*";
    size_t assume = lexr.assume(file);

    KeywordMap kwd = CreateIRKeywordMap();
    
    lexr.SetKeywords(kwd);

    return lexr.lex_perf(file, assume);
}

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

int main(){
    LexerFile file = "examples/inertia.inr";
    LexerOutput lex = GetLexedFile(file);

    for(const Token& tok : lex){
        std::cout<<tok.view_str(*lex.file)<<std::endl;
    }

    return 0;
}