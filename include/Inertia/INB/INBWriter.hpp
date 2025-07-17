#ifndef INBWRITER_HPP
#define INBWRITER_HPP

#include "Inertia/Mem/Archmem.hpp"
#include "Inertia/Mem/Memstream.hpp"
#include "Inertia/Target/Architecture.hpp"
#include <cstdint>

namespace Inertia{
namespace INB{

// INB_MAGIC = MachineEndian == Endian::IN_LITTLE_ENDIAN ? 'BNI@' : '@INB';
extern const uint32_t INB_MAGIC;
constexpr uint32_t IDRESTRICT_PLACEHOLDER = 0xDEADBEEF;

constexpr uint8_t FUNC_PREFIX = 0x02;

constexpr uint8_t NEST_OPEN = 0x7b;
constexpr uint8_t NEST_CLOSE = 0x7d;

constexpr uint8_t VAA_PREFIX = 0x20;

namespace ControlFlow{
    constexpr uint8_t PREFIX = 0x03;
    enum{
        RET_DIRECT = 0x01,
        RET_ADDRESS = 0x02
    };
}

namespace MemoryInstruction{
    constexpr uint8_t PREFIX = 0x01;
    enum{
        SPACE = 0x01,
        STACK = 0x02,
    };
}

constexpr const char* SECTION_VATAB = ".vatab:";
constexpr const char* SECTION_DATA = ".data_sect:";
constexpr const char* SECTION_CODE = ".code:";
constexpr const char* SECTION_ASM = ".asm:";

enum class INBHeaderFlags : uint8_t{
    FLAG_NONE = 0x0,
    FLAG_PIC = 0x1,
    FLAG_ASM = 0x2,
};

enum class ASMFlags : uint8_t{
    STRING = 0x0, RAW = 0x1
};

#pragma pack(push, 1)
struct InertiaHeader{
    uint8_t magic[4];
    Endian endian;
    uint8_t version = 1;
    uint8_t flags = 0;
    uint8_t reserved = 0;

    InertiaHeader(Endian _endian = MachineEndian) : endian(_endian){
        magic[0] = '@';
        magic[1] = 'I';
        magic[2] = 'N';
        magic[3] = 'B';
    }
};
struct InertiaHeaderOffsets{
    uint32_t offset_vatab;
    uint32_t offset_data_sect;
    uint64_t offset_code;
};
#pragma pack(pop)

// [f:1]\n[len]\n[bytes]\0
struct ASMSection{
    ASMFlags flags;

    std::vector<uint8_t> bytes;

    ASMSection(ASMFlags f, const std::string& str) : flags(f), bytes(str.data(), str.data() + str.size()){};
    ASMSection(ASMFlags f, const std::vector<uint8_t>& b) : flags(f), bytes(b){};
};

enum class INBTypes : uint8_t{
    BYTE = 1, WORD = 2, DWORD = 4, QWORD = 8, PTR = 16, FLOAT = 3, DOUBLE = 5
};

enum class VAFlags : uint8_t{
    GLOBAL = 0x1, LOCAL = 0x2, PREALLOCATED = 0x4 // preallocated meaning points to AllocatedData
};

enum class PDFlags : uint8_t{
    GLOBAL = 0x1, LOCAL = 0x2,
    READONLY = 0x4, RESERVE = 0x8
}; // AllocatedData
/* READONLY: RODATA, RESERVE: BSS */

inline VAFlags operator|(VAFlags lhs, VAFlags rhs){
    return (VAFlags)((uint8_t)lhs | (uint8_t)rhs);
}

inline PDFlags operator|(PDFlags lhs, PDFlags rhs){
    return (PDFlags)((uint8_t)lhs | (uint8_t)rhs);
}

struct VirtualAddress{
    VAFlags flag;
    uint32_t uniqueID;
    std::string name;

    VirtualAddress(const std::string& _name, VAFlags flags) : flag(flags), name(_name){};
};

struct AllocatedData{
    uint32_t AddressID;
    INBTypes type;
    PDFlags flag;
    size_t reserve;
    std::vector<uint8_t> data;

    AllocatedData(PDFlags flags, INBTypes _t, uint32_t id, size_t _reserve) noexcept : AddressID(id), type(_t), flag(flags), reserve(_reserve){}
    AllocatedData(PDFlags flags, INBTypes _t, uint32_t id, const std::vector<uint8_t>& _data) : AddressID(id), type(_t), flag(flags), data(_data){};
};

/* STRUCTURE */ /*
@INB, ENDIAN, VERSION, FLAGS, RESERVED, OFFSETS
ADDRESSES (.vatab): ADDRESS [flag:1]\n[id:4]\n[length:4]\n[name:length]
DATA SECTIONS: NORMAL, RESERVE, READONLY
CODE [instruction] [instruction]..
ASM
*/

class INBWriter{
    MemoryStream mss;
    InertiaHeader hdr;
    std::vector<VirtualAddress> vaas;
    std::vector<AllocatedData> data;
    std::vector<ASMSection> asms;
    std::vector<uint8_t> codeBytes;
    uint32_t curID = 0;
    Endian endian;

    inline void WriteData(AllocatedData& dt){
        mss<<(char)dt.flag<<'\x0A';
        mss<<(char)dt.type<<'\x0A';

        mss.write_endian(dt.AddressID);

        mss<<'\x0A';

        mss.write_endian<uint32_t>(dt.data.size());

        mss<<'\x0A';

        for(uint8_t i : dt.data){
            mss<<(char)i;
        }

        mss<<'\0';
    }
    inline void WriteAddress(VirtualAddress& va){
        mss<<(char)va.flag<<'\x0A';
        mss.write_endian<uint32_t>(va.uniqueID);
        mss<<'\x0A';
        mss.write_endian<uint32_t>(va.name.length());
        mss<<'\x0A';
        mss<<va.name<<'\0';
    }
    inline void WriteOffsets(InertiaHeaderOffsets& offsets){
        mss.write_endian<uint32_t>(offsets.offset_vatab);
        mss.write_endian<uint32_t>(offsets.offset_data_sect);
        mss.write_endian<uint64_t>(offsets.offset_code);
    }
public:
    INBWriter(Endian _endian = MachineEndian) noexcept : mss(32), endian(_endian){};
    
    INBWriter(const char* fileName, Endian _endian = MachineEndian) noexcept : mss(fileName, true), endian(_endian){};

    INBWriter(INBWriter& rhs) = delete;
    INBWriter& operator=(INBWriter& rhs) = delete;

    inline void setup() noexcept{
        hdr = InertiaHeader(endian);
        mss.SetEndianess(endian);
    }

    inline Endian get_endian() const noexcept{
        return endian;
    }

    inline void set_flag(INBHeaderFlags f) noexcept{
        hdr.flags |= (uint8_t)f;
    }

    inline uint32_t add_asm_str(const std::string& str){
        hdr.flags |= (uint8_t)INBHeaderFlags::FLAG_ASM;
        asms.emplace_back(ASMFlags::STRING, str);
        return asms.size()-1;
    }

    inline uint32_t add_asm_bytes(const std::vector<uint8_t>& vec){
        hdr.flags |= (uint8_t)INBHeaderFlags::FLAG_ASM;
        asms.emplace_back(ASMFlags::STRING, vec);
        return asms.size()-1;
    }

    inline void address(VirtualAddress& addy){
        addy.uniqueID = curID;
        curID++;
        if(curID == IDRESTRICT_PLACEHOLDER){
            // Future linker, address placeholder.
            curID++;
        }
        vaas.push_back(addy);
    }

    inline void address_implicit(const VirtualAddress addy){
        curID++;
        if(curID == IDRESTRICT_PLACEHOLDER){
            // Future linker, address placeholder.
            curID++;
        }
        vaas.push_back(addy);
    }

    // assumes hardware endian
    inline void add_data(INBTypes type, uint32_t id, const std::vector<uint8_t>& bytes, PDFlags flags = PDFlags::LOCAL){
        if(bytes.empty()) return;
        data.emplace_back(flags, type, id, bytes);
    }

    // assumes hardware endian
    inline void add_rodata(INBTypes type, uint32_t id, const std::vector<uint8_t>& bytes, PDFlags flags = PDFlags::LOCAL){
        if(bytes.empty()) return;
        data.emplace_back(flags | PDFlags::READONLY, type, id, bytes);
    }

    inline void add_reserve(INBTypes type, uint32_t id, size_t reserve, PDFlags flags = PDFlags::LOCAL){
        data.emplace_back(flags | PDFlags::RESERVE, type, id, reserve);
    }

    inline void insert_code(uint8_t* start, size_t size){
        codeBytes.insert(codeBytes.end(), start, start + size);
    }

    inline void instruction(uint8_t* data, uint8_t* end){
        codeBytes.push_back('[');
        insert_code(data, end - data);
        codeBytes.push_back(']');
    }

    void finish();

    inline void new_instr(){
        codeBytes.push_back('[');
    }

    inline void emit(uint8_t prefix, uint8_t inst){
        codeBytes.push_back(prefix);
        codeBytes.push_back(inst);
    }

    inline void ins_va(const VirtualAddress& addy){
        codeBytes.push_back(VAA_PREFIX);
        InsertEndian<uint8_t, uint32_t>(codeBytes, addy.uniqueID, endian);
    }

    inline void ins_type(INBTypes type){
        codeBytes.push_back((uint8_t)type);
    }

    // ptr must have a VA unique id after it
    inline void ins_type(INBTypes type, uint64_t value){
        codeBytes.push_back((uint8_t)type);
        switch(type){
            case INBTypes::BYTE:
                codeBytes.push_back((uint8_t)value);
                break;
            case INBTypes::WORD:
                InsertEndian<uint8_t, uint16_t>(codeBytes, (uint16_t)value, endian);
                break;
            case INBTypes::FLOAT:
                [[fallthrough]];
            case INBTypes::DWORD:
                InsertEndian<uint8_t, uint32_t>(codeBytes, (uint32_t)value, endian);
                break;
            case INBTypes::DOUBLE:
                [[fallthrough]];
            case INBTypes::QWORD:
                InsertEndian<uint8_t, uint64_t>(codeBytes, (uint64_t)value, endian);
                break;
            case INBTypes::PTR:
                codeBytes.push_back(VAA_PREFIX);
                InsertEndian<uint8_t, uint32_t>(codeBytes, (uint32_t)value, endian);
                break;;
        }
    }

    inline void finish_instr(){
        codeBytes.push_back(']');
    }

    inline void start_func(INBTypes ret, const std::string& name){
        codeBytes.push_back(FUNC_PREFIX);
        codeBytes.push_back((uint8_t)ret);
        codeBytes.insert(codeBytes.end(), name.data(), name.data() + name.size());
        codeBytes.push_back('\0');
    }

    inline void func_arg(INBTypes type, const VirtualAddress& va){
        codeBytes.push_back((uint8_t)type);
        codeBytes.push_back(VAA_PREFIX);
        InsertEndian<uint8_t, uint32_t>(codeBytes, va.uniqueID, endian);
    }

    inline void open_nest(){
        new_instr();
        codeBytes.push_back(NEST_OPEN);
        finish_instr();
    }

    inline void close_nest(){
        new_instr();
        codeBytes.push_back(NEST_CLOSE);
        finish_instr();
    }

    inline void ret_direct(INBTypes type, uint64_t value){
        new_instr();
        codeBytes.push_back(ControlFlow::PREFIX);
        codeBytes.push_back(ControlFlow::RET_DIRECT);
        ins_type(type, value);

        finish_instr();
    }

    inline void ret_address(INBTypes type, const VirtualAddress& va){
        new_instr();
        codeBytes.push_back(ControlFlow::PREFIX);
        codeBytes.push_back(ControlFlow::RET_DIRECT);
        func_arg(type, va);
        finish_instr();
    }

    ~INBWriter() = default;

};

}
}

#endif // INBWRITER_HPP
