#include "Inertia/ELF/ELFWriter.hpp"

namespace Inertia{
namespace ELF{

    void ELFWriter::CreateHeader(ELF_Machine machine, ELF_ABI abi, ELF_Type type, Bitness bits, Endian endian){
        if(!header.empty()) header.clear();
        mss.SetEndianess(endian);
        end = endian;
        ELF_Ident idt(bits, endian, abi);
        tp = type;

        header.reserve(0x40);

        InsertNoEndian(idt.magic);
        
        InsertIntoHeader(idt.bit);
        InsertIntoHeader(idt.endian);
        InsertIntoHeader(idt.version);
        InsertIntoHeader(idt.abi_t);
        InsertIntoHeader(idt.abi_v);

        InsertNoEndian(idt.pad);
        
        InsertIntoHeader(type);
        InsertIntoHeader(machine);
        InsertIntoHeader(ELF_Version::Current);

        uint16_t ehsize;
        uint16_t shentsize;

        if(bits == Bitness::M_32_BIT){
            ehsize = 52;
            shentsize = 40;

            track.offset_e_entry = header.size();
            ZeroHeader<uint32_t>();

            track.offset_e_phoff = header.size();
            ZeroHeader<uint32_t>();

            track.offset_e_shoff = header.size();
            ZeroHeader<uint32_t>();
        }
        else{
            ehsize = 64;
            shentsize = 64;

            track.offset_e_entry = header.size();
            ZeroHeader<uint64_t>();

            track.offset_e_phoff = header.size();
            ZeroHeader<uint64_t>();

            track.offset_e_shoff = header.size();
            ZeroHeader<uint64_t>();
        }

        InsertIntoHeader<uint32_t>(0);

        InsertIntoHeader(ehsize);

        uint16_t phentsize = 0;
        InsertIntoHeader(phentsize);

        uint16_t phnum = 0;
        track.offset_e_phnum = header.size();
        InsertIntoHeader(phnum);

        InsertIntoHeader(shentsize);

        track.offset_e_shnum = header.size();
        ZeroHeader<uint16_t>();

        track.offset_e_shstrndx = header.size();
        ZeroHeader<uint16_t>();
    }

}
}