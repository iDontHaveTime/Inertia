#include "Inertia/Codegen/Codegen.hpp"
#include "Inertia/IR/IRParser.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/Lexer.hpp"
#include "Inertia/IR/IRKeywords.hpp"
#include "Inertia/Lexer/LexerOutput.hpp"
#include "Inertia/Lexer/TokenType.hpp"
#include "Inertia/Mem/Memstream.hpp"
#include "Inertia/Target/x8664.hpp"
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

    IRCodegen codegen;

    MemoryStream mss("examples/inertia.asm");

    Targetx86_64 tg(talloc.get_arena());

    tg.PIC = true;

    tg.abi.get_sysv(tg.regs);

    std::cout<<tg.regs->rax->l64b->child->name<<std::endl;

    codegen.codegen_assembly(mss, frame, &tg);

    return 0;
}