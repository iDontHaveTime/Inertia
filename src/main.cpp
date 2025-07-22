#include "Inertia/INB/INBWriter.hpp"
#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/Lexer.hpp"
#include "Inertia/IR/IRKeywords.hpp"
#include "Inertia/Lexer/LexerOutput.hpp"
#include "Inertia/Target/Architecture.hpp"
#include "Inertia/ELF/ELFWriter.hpp"
#include <cstddef>
#include <cstdio>

using namespace Inertia;

LexerOutput GetLexedFile(const char* fname){
    LexerFile file(fname);
    
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
    INB::INBWriter inb("examples/irbin.inb");

    inb.setup();

    inb.set_flag(INB::INBHeaderFlags::FLAG_PIC);

    inb.address_implicit({"xyz", INB::VAFlags::GLOBAL});
    inb.address_implicit({"hello", INB::VAFlags::GLOBAL});
    inb.address_implicit({"another", INB::VAFlags::LOCAL});

    INB::VirtualAddress str0("str0", INB::VAFlags::GLOBAL | INB::VAFlags::PREALLOCATED);

    INB::VirtualAddress integer("int", INB::VAFlags::GLOBAL | INB::VAFlags::PREALLOCATED);

    INB::VirtualAddress res("reserved", INB::VAFlags::GLOBAL | INB::VAFlags::PREALLOCATED);

    INB::VirtualAddress space("space", INB::VAFlags::LOCAL);

    INB::VirtualAddress argc("argc", INB::VAFlags::LOCAL);
    INB::VirtualAddress argv("argv", INB::VAFlags::LOCAL);

    inb.address(str0);
    inb.address(integer);
    inb.address(res);
    inb.address(space);
    inb.address(argc);
    inb.address(argv);

    inb.add_data(
        INB::INBTypes::DWORD, 
        integer.uniqueID, 
        {0x2A, 0x00, 0x00, 0x00}
    );

    inb.add_rodata(
        INB::INBTypes::BYTE, 
        str0.uniqueID, 
        {'H', 'e', 'l', 'l', 'o', '\0'}
    );
    
    inb.add_reserve(INB::INBTypes::PTR, res.uniqueID, 2);

    // inb.add_asm_str("movq %rax, %rbx");

    inb.new_instr();

    inb.start_func(INB::INBTypes::DWORD, INB::PDFlags::GLOBAL, "main");
    inb.func_arg(INB::INBTypes::DWORD, argc);
    inb.func_arg(INB::INBTypes::PTR, argv);

    inb.finish_instr();

    inb.open_nest();



    inb.ret_direct(INB::INBTypes::DWORD, 0);

    inb.close_nest();

    inb.finish();

    return 0;
}