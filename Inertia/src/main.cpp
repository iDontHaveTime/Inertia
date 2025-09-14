#include "Inertia/Assembly/ASMPrinter.hpp"
#include "Inertia/Assembly/Generic/ASMPrinterGen.hpp"
#include "Inertia/Compiler/Flags.hpp"
#include "Inertia/Debug/DebugInfo.hpp"
#include "Inertia/IR/Frame.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/IR/IRBuilder.hpp"
#include "Inertia/IR/IRPrinter.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Lexer/LexerConfig.hpp"
#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/Lexer.hpp"
#include "Inertia/Lowering/InstrSelMan.hpp"
#include "Inertia/Lowering/LoweredOut.hpp"
#include "Inertia/Target/TargetCodegen.hpp"
#include "Inertia/Target/TargetManager.hpp"
#include "Inertia/Target/TargetParser.hpp"
#include "Inertia/Target/Triple.hpp"
#include <fstream>

using namespace Inertia;

/*
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
    LexerFile file("examples/inertia.inr");

    LexerOutput out = GetLexedFile(file);

    TypeAllocator talloc;

    IRParser parser(&file);

    auto frame = parser.parse_tokens(out, talloc);
}
*/
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

void makeMainFunc(IRBuilder& builder){
    auto funcmain = builder.buildFunction("main", builder.getAllocator()->getInteger(32), Function::LinkageType::EXTERNAL);

    builder.addArg(funcmain, builder.getAllocator()->getInteger(32));
    builder.addArg(funcmain, builder.getAllocator()->getPointer(
        builder.getAllocator()->getPointer(builder.getAllocator()->getInteger(8))
    ));

    auto entry = builder.buildBlock("entry", funcmain);

    builder.buildReturn(entry, builder.newSSAConst(funcmain, builder.getAllocator()->getInteger(32), 0));
}

void makeAlignedFunc(IRBuilder& builder){
    auto funcaligned = builder.buildFunction("aligned", builder.getAllocator()->getPointer(builder.getAllocator()->getVoid()), Function::LinkageType::EXTERNAL, Function::MANUAL_ALIGN, 32);

    auto entry = builder.buildBlock("entry", funcaligned);

    builder.buildReturn(entry, builder.newSSAConst(funcaligned, builder.getAllocator()->getVoid(), 0));
}

void makeLocalFunc(IRBuilder& builder){
    auto funclocal = builder.buildFunction("local", builder.getAllocator()->getPointer(builder.getAllocator()->getVoid()), Function::LinkageType::INTERNAL);

    auto entry = builder.buildBlock("entry", funclocal);

    builder.buildReturn(entry, builder.newSSAConst(funclocal, builder.getAllocator()->getPointer(builder.getAllocator()->getVoid()), -1));
}

int main(){
    /* "Compiling" the DSL */
    compile_x86t();
    compile_aarch64t();
    // lines above have nothing to do with the actual IR compiling

    /* Lowering the IR to assembly */
    TargetTriple ttrip("x86_64-linux-gnu");

    ASMPrinter printer;

    printer.load_target(ttrip);

    TargetManager tm;
    tm.load_target(ttrip);

    InstructionSelectorManager ism;
    ism.load_target(ttrip);

    TypeAllocator talloc(0x4000);
    Frame newFrame(&ttrip);
    IRBuilder builder(&talloc, &newFrame);

    makeMainFunc(builder);
    makeAlignedFunc(builder);
    makeLocalFunc(builder);

    IRPrinter irprint;

    std::ofstream outir("examples/represent.inr");
    irprint.output(newFrame, outir);

    DebugInfo dbi;
    CompilerFlags flags;
    flags.PIC = true;

    LoweredOutput cdg;
    cdg.debug = &dbi;
    cdg.ttriple = &ttrip;
    cdg.cflags = &flags;

    ism.lower(newFrame, cdg);

    LoweredOutput cdgNoPIC;
    cdgNoPIC.debug = &dbi;
    cdgNoPIC.ttriple = &ttrip;
    flags.PIC = false;
    cdgNoPIC.cflags = &flags;

    ism.lower(newFrame, cdgNoPIC);

    std::ofstream outpic("examples/output.S");
    printer.output(cdg, outpic);

    std::ofstream outnopic("examples/outputnoPIC.S");
    printer.output(cdgNoPIC, outnopic);

    return 0;
}
