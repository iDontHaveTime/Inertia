#include "Inertia/INB/INBWriter.hpp"

namespace Inertia{
namespace INB{

const uint32_t INB_MAGIC = MachineEndian == Endian::IN_LITTLE_ENDIAN ? 'BNI@' : '@INB';

void INBWriter::finish(){
    mss.clear();

    mss.write(hdr);

    InertiaHeaderOffsets offsets;

    WriteOffsets(offsets);

    offsets.offset_vatab = mss.size();
    mss<<SECTION_VATAB<<'\0'; // its a stream after all

    for(VirtualAddress& va : vaas){
        // [flag:1]\n[id:4]\n[length:4]\n[name:length]
        WriteAddress(va);
    }

    mss<<'\x0A';

    offsets.offset_data_sect = mss.size();
    mss<<SECTION_DATA<<'\0';

    mss<<"normal:"<<'\0';
    // [flag:1]\n[type:1]\n[address:4]\n[RESERVE/DATA SIZE:4](only if not reserve)\n[DATA:X]
    for(AllocatedData& dt : data){
        if(!((uint8_t)dt.flag & (uint8_t)PDFlags::READONLY) && !((uint8_t)dt.flag & (uint8_t)PDFlags::RESERVE)){
            WriteData(dt);
        }
    }

    mss<<"readonly:"<<'\0';

    for(AllocatedData& dt : data){
        if((uint8_t)dt.flag & (uint8_t)PDFlags::READONLY){
            WriteData(dt);
        }
    }

    mss<<"reserve:"<<'\0';

    for(AllocatedData& dt : data){
        if((uint8_t)dt.flag & (uint8_t)PDFlags::RESERVE){
            mss<<(char)dt.flag<<'\x0A';
            mss<<(char)dt.type<<'\x0A';

            mss.write_endian(dt.AddressID);

            mss<<'\x0A';

            mss.write_endian<uint32_t>(dt.reserve);

            mss<<'\0';
        }
    }


    if(mss.size() % 16 != 0){
        size_t mod = 16 - (mss.size() % 16);
        if(mod > 1){
            mss<<'[';
            mod--;
        }
        for(size_t i = 0; i < mod; i++){
            if(i + 1 == mod){
                mss<<']';
                break;
            }
            mss<<'\0';
        }
    }

    offsets.offset_code = mss.size();
    mss<<SECTION_CODE<<'\0';

    mss.write(codeBytes.data(), codeBytes.size());

    mss<<'\0';

    if(hdr.flags & (uint8_t)INBHeaderFlags::FLAG_ASM){
        mss<<SECTION_ASM<<'\0';
        for(size_t i = 0; i < asms.size(); i++){
            ASMSection& sect = asms[i];
            mss<<(char)sect.flags<<'\x0A';
            mss.write_endian<uint32_t>(i);
            mss<<'\x0A';
            mss.write_endian<uint32_t>(sect.bytes.size());
            mss<<'\x0A';
            mss.write(sect.bytes.data(), sect.bytes.size());
            mss<<'\0';
        }
    }

    size_t pos = mss.size();
    mss.seek(sizeof(InertiaHeader));
    WriteOffsets(offsets);
    mss.seek(pos);
}

}
}