#ifndef ELFWRITER_HPP
#define ELFWRITER_HPP

#include "../Mem/Memstream.hpp"
#include "Inertia/Mem/Archmem.hpp"
#include <cstdint>
#include <cstring>

namespace Inertia{
namespace ELF{

enum class ELF_Type : uint16_t{
    None = 0,
    Relocatable = 1,
    Executable = 2,
    SharedObject = 3,
    Core = 4,
};

enum class ELF_ABI : uint8_t{
    SystemV = 0,
    HP_UX = 1,
    NetBSD = 2,
    Linux = 3,
    Solaris = 6,
    AIX = 7,
    IRIX = 8,
    FreeBSD = 9,
    TRU64 = 10,
    NovellModesto = 11,
    OpenBSD = 12,
    ARM_AEABI = 64,
    ARM = 97,
    Standalone = 255
};

enum class ELF_Machine : uint16_t {
    None = 0,
    M32 = 1,
    SPARC = 2,
    Intel386 = 3,
    Motorola68000 = 4,
    Motorola88000 = 5,
    Intel860 = 7,
    MIPS = 8,
    S370 = 9,
    MIPS_RS3_LE = 10,
    ARM = 40,
    SuperH = 42,
    Sparc32Plus = 43,
    IA_64 = 50,
    PowerPC = 20,
    PowerPC64 = 21,
    VAX = 75,
    Alpha = 41,
    AArch64 = 183,
    TilePro = 188,
    MicroBlaze = 189,
    Nios2 = 113,
    ARC = 0xF3,
    BPF = 247,
    RISCV = 243,
    LoongArch = 258,
    X86_64 = 62
};

namespace ELFSectionFlags{
    constexpr uint64_t NONE = 0;
    constexpr uint64_t WRITE = 0x1;
    constexpr uint64_t ALLOCATE = 0x2;
    constexpr uint64_t EXECUTABLE = 0x4;
    constexpr uint64_t MERGEABLE = 0x10;
    constexpr uint64_t STRING_DATA = 0x20;
    constexpr uint64_t INFO_LINK = 0x40;
    constexpr uint64_t LINK_ORDER = 0x80;
    constexpr uint64_t NON_CONFORMING_OS = 0x100;
    constexpr uint64_t GROUP = 0x200;
    constexpr uint64_t THREAD_LOCAL_STORAGE = 0x400;
    constexpr uint64_t COMPRESSED = 0x800;
    constexpr uint64_t OS_SPECIFIC_MASK = 0x0ff00000;
    constexpr uint64_t PROCESSOR_SPECIFIC_MASK = 0xf0000000;
}

constexpr uint64_t ELF_SEC_TEXT = ELFSectionFlags::ALLOCATE | ELFSectionFlags::EXECUTABLE;
constexpr uint64_t ELF_SEC_DATA = ELFSectionFlags::ALLOCATE | ELFSectionFlags::WRITE;
constexpr uint64_t ELF_SEC_RODATA = ELFSectionFlags::ALLOCATE | ELFSectionFlags::STRING_DATA;
constexpr uint64_t ELF_SEC_BSS = ELFSectionFlags::ALLOCATE | ELFSectionFlags::WRITE;

enum class ELF_Version : uint32_t{
    Current = 1
};

#pragma pack(push, 1)
struct ELF_Ident{
    uint8_t magic[4];
    Bitness bit;
    Endian endian;
    uint8_t version = 1;
    ELF_ABI abi_t;
    uint8_t abi_v = 0;
    uint8_t pad[7] = {0};

    ELF_Ident() = default;
    ELF_Ident(Bitness bits, Endian end, ELF_ABI abi) : bit(bits), endian(end), abi_t(abi){
        magic[0] = 0x7F;
        magic[1] = 'E';
        magic[2] = 'L';
        magic[3] = 'F';
    };
};
#pragma pack(pop)

struct ELFSection{
    std::string name;
    std::vector<uint8_t> data;
    uint32_t type = 1;
    uint64_t flags = 0;
    uint64_t addr = 0;
    uint64_t align = 1;
    uint64_t entsize = 0;
};

struct ELF_Tracker{
    size_t offset_e_shoff; // byte offset in header vector to e_shoff
    size_t offset_e_shnum; // offset to e_shnum
    size_t offset_e_shstrndx; // offset to e_shstrndx
    size_t offset_e_phoff;
    size_t offset_e_phnum;
    size_t offset_e_entry;
};

class ELFWriter{
    MemoryStream mss;
    std::vector<uint8_t> header;
    std::vector<ELFSection> sections;
    ELF_Tracker track;
    ELF_Type tp;
    Endian end;

    template<typename T>
    inline void InsertNoEndian(const T& tmp){
        header.insert(header.end(), (uint8_t*)&tmp, (uint8_t*)&tmp + sizeof(T));
    }

    template<typename T>
    inline void InsertIntoHeader(const T& tmp){
        if(end != MachineEndian){
            uint8_t memf[sizeof(T)];
            std::memcpy(memf, &tmp, sizeof(T));
            Inertia::FlipMemEndian(memf, sizeof(T));
            header.insert(header.end(), memf, memf + sizeof(T));
            return;
        }
        header.insert(header.end(), (uint8_t*)&tmp, (uint8_t*)&tmp + sizeof(T));
    }
    template<typename T>
    inline void PatchHeader(size_t offset, T value){
        if(end != MachineEndian){
            uint8_t memf[sizeof(T)];
            std::memcpy(memf, &value, sizeof(T));
            Inertia::FlipMemEndian(memf, sizeof(T));
            std::memcpy(&header[offset], memf, sizeof(T));
            return;
        }
        std::memcpy(&header[offset], &value, sizeof(T));
    }
    template<typename T>
    inline void ZeroHeader(int count = 1){
        if(count < 0) return;
        const T zero = 0;
        for(int i = 0; i < count; i++){
            InsertIntoHeader(zero);
        }
    }
public:

    inline MemoryStream& MemStream() noexcept{
        return mss;
    }

    ELFWriter() noexcept : mss(64){};

    // Memory stream shouldn't be copied
    ELFWriter(ELFWriter& rhs) = delete;
    ELFWriter& operator=(ELFWriter& rhs) = delete;

    ELFWriter(const char* fileName) noexcept : mss(fileName, true){};

    inline void AddSection(const std::string& name, const std::vector<uint8_t>& code, uint32_t type = 1, uint64_t flags = ELFSectionFlags::NONE, uint64_t align = 1){
        ELFSection sec;
        sec.name = name;
        sec.data = code;
        sec.type = type;
        sec.flags = flags;
        sec.align = align;
        sections.push_back(std::move(sec));
    }

    void CreateHeader(ELF_Machine machine, ELF_ABI abi, ELF_Type type, Bitness bits, Endian endian);

    inline void WriteHeader() noexcept{
        if(!mss){
            return;
        }
        size_t len = mss.size();
        mss.seek(0);
        mss.write(header.data(), header.size());
        if(len != 0){
            mss.seek(len);
        }
    }

    ~ELFWriter() = default;
};


}
}

#endif // ELFWRITER_HPP
