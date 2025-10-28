#include "inr/MC/MCELFWriter.hpp"
#include "inr/Support/Alloc.hpp"
#include "inr/Support/Binary.hpp"
#include "inr/Target/Triple.hpp"
#include <cstring>

namespace inr{


size_t MCELFWriter::write(){
    if(!valid()) return 0;

    size_t pos = 0;

    Triple triple = get_triple();

    bool flip_endians = endian::native != get_arch_endian(triple.get_arch());
    size_t amount_to_write = 0;

    char data_to_write[std::max(sizeof(elf_header_64), sizeof(elf_header_32))];


    if(is_arch_64bit(triple.get_arch())){
        amount_to_write = sizeof(elf_header_64);
        
        memcpy(data_to_write, hdr, sizeof(elf_header_64));

        elf_header_64* elf = (elf_header_64*)data_to_write;
        if(flip_endians) elf->flip_fields();
    }
    else{
        amount_to_write = sizeof(elf_header_32);

        memcpy(data_to_write, hdr, sizeof(elf_header_32));

        elf_header_32* elf = (elf_header_32*)data_to_write;
        if(flip_endians) elf->flip_fields();
    }

    pos += os.write(data_to_write, 1, amount_to_write);

    return pos;
}

void MCELFWriter::end_elf(){
    Triple triple = get_triple();

    allocator* mem = get_allocator();

    if(is_arch_64bit(triple.get_arch())){
        mem->free((elf_header_64*)hdr);
    }
    else{
        mem->free((elf_header_32*)hdr);
    }
}

void MCELFWriter::new_elf(){

    allocator* mem = get_allocator();

    if(!mem){
        mem = &static_allocator;
    }

    Triple triple = get_triple();

    if(is_arch_64bit(triple.get_arch())){
        hdr = mem->alloc<elf_header_64>(
            get_arch_endian(triple.get_arch()),
            triple_to_elf_abi(triple),
            elf_type::ELF_REL,
            triple_to_elf_isa(triple)
        );
    }
    else{
        hdr = mem->alloc<elf_header_32>(
            get_arch_endian(triple.get_arch()),
            triple_to_elf_abi(triple),
            elf_type::ELF_REL,
            triple_to_elf_isa(triple)
        );
    }


}

}