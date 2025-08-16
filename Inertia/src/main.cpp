#include "Inertia/Assembly/ASMPrinter.hpp"
#include "Inertia/IR/Frame.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/IR/IRBuilder.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Lexer/LexerConfig.hpp"
#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/Lexer.hpp"
#include "Inertia/IR/IRKeywords.hpp"
#include "Inertia/Lexer/LexerOutput.hpp"
#include "Inertia/Lexer/TokenType.hpp"
#include "Inertia/Lowering/LoweredOut.hpp"
#include "Inertia/Target/TargetCodegen.hpp"
#include "Inertia/Target/TargetManager.hpp"
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
    LexerConfig cfg;
    cfg.configure(lexr, Inertia::LexerPreConfigs::TARGET);

    LexerFile lfile("Inertia/include/Inertia/Target/AArch64/TargetAArch64.int");

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

int compile_x86t(){
    Lexer lexr;
    LexerConfig cfg;
    cfg.configure(lexr, Inertia::LexerPreConfigs::TARGET);

    LexerFile lfile("Inertia/include/Inertia/Target/x86/Targetx86.int");
    LexerFile regfile("Inertia/include/Inertia/Target/x86/Targetx86reg.int");

    if(!lfile || !regfile){
        std::cerr<<"Error in target x86 codegen, file not found"<<std::endl;
        return true;
    }

    TargetParser tp(&lfile);
    //tp.cpp_injections = true;
    auto lxo = lexr.lex(lfile);
    auto lxo2 = lexr.lex(regfile);

    auto out = tp.parse({&lxo2, &lxo});

    TargetCodegen cg(out);

    if(cg.output()){
        std::cerr<<"Error in target x86 codegen"<<std::endl;
        return 1;
    }
    return 0;
}

int main(){
    compile_x86t();
    compile_aarch64t();
    ASMPrinter printer;

    printer.load_target(TargetType::x86);

    TargetManager tm;
    tm.load_target(TargetType::x86);

    TypeAllocator talloc;
    Frame newFrame;
    IRBuilder builder(&talloc, &newFrame);

    builder.buildFunction("main");
    builder.buildFunction("aligned", Function::MANUAL_ALIGN, 32);
    builder.buildFunction("local", Function::LOCAL);

    LoweredOutput cdg;

    printer.set_path("examples/output.S");

    printer.output(cdg);

    return 0;
}