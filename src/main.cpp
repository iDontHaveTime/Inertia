#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/Lexer.hpp"
#include "Inertia/IR/IRKeywords.hpp"
#include "Inertia/Lexer/LexerOutput.hpp"
#include "Inertia/Lexer/TokenType.hpp"
#include "Inertia/Target/TargetCodegen.hpp"
#include "Inertia/Target/TargetKeywords.hpp"
#include "Inertia/Target/TargetParser.hpp"
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

void temp(){
    //LexerFile file("examples/inertia.inr");

    //LexerOutput out = GetLexedFile(file);

    //TypeAllocator talloc;

    //IRParser parser(&file);

    //auto frame = parser.parse_tokens(out, talloc);
}

int compile_aarch64t(){
    Lexer lexr;
    lexr.line_comment = TokenType::SlashSlash;
    lexr.multiline_end = TokenType::StarSlash;
    lexr.multiline_start = TokenType::SlashStar;
    lexr.throw_eof = true;

    lexr.SetKeywords(CreateTargetKeywordMap());

    LexerFile lfile("include/Inertia/Target/AArch64/TargetAArch64.int");

    TargetParser tp(&lfile);

    auto lxo = lexr.lex(lfile);
    
    auto out = tp.parse({&lxo});

    TargetCodegen cg(out);

    if(cg.output()){
        std::cerr<<"Error in target AArch64 codegen"<<std::endl;
        return 1;
    }
    return 0;
}

int compile_x8664t(){
    Lexer lexr;
    lexr.line_comment = TokenType::SlashSlash;
    lexr.multiline_end = TokenType::StarSlash;
    lexr.multiline_start = TokenType::SlashStar;
    lexr.throw_eof = true;

    lexr.SetKeywords(CreateTargetKeywordMap());

    LexerFile lfile("include/Inertia/Target/x86/Targetx8664.int");
    LexerFile regfile("include/Inertia/Target/x86/Targetx8664reg.int");

    TargetParser tp(&lfile);
    auto lxo = lexr.lex(lfile);
    auto lxo2 = lexr.lex(regfile);

    auto out = tp.parse({&lxo2, &lxo});

    TargetCodegen cg(out);

    if(cg.output()){
        std::cerr<<"Error in target x8664 codegen"<<std::endl;
        return 1;
    }
    return 0;
}

int main(){
    compile_x8664t();
    compile_aarch64t();

    return 0;
}