#include "Inertia/Mem/Memstream.hpp"
#include "Inertia/Mem/Autopointer.hpp"

namespace Inertia{


AutoPointer<char, AutoPointerType::Array> MemoryStream::to_buffer(){
    if(dir == StreamDirection::None) return {};
    AutoPointer<char, AutoPointerType::Array> mem(new char[length]);
    if(!mem.get()){
        return {};
    }

    if(dir == StreamDirection::Memory){
        if(!mems) return {};
        memcpy(mem, mems, length);
    }
    else{
        if(!fst) return {};
        fread(mem, 1, length, fst);
    }

    return AutoPointer<char, AutoPointerType::Array>(std::move(mem));
}

MemoryStream& MemoryStream::operator<<(double n) noexcept{
    if(bin){
        write(&n, sizeof(double));
    }
    else{
        char buff[64] = {0};
        sprintf(buff, "%f", n);
        for(size_t i = 63; i > 0; i--){
            if(buff[i] == '0'){
                buff[i] = '\0';
            }
            else if(buff[i] == '.'){
                buff[i] = '\0';
                break;
            }
            else{
                if(buff[i] != '\0') break;
            }
        }
        write(buff, strlen(buff));
    }
    return *this;
}

MemoryStream::MemoryStream(MemoryStream&& rhs) noexcept{
    if(this != &rhs){
        close();
        dir = rhs.dir;
        rhs.dir = StreamDirection::None;
        mems = rhs.mems;
        rhs.mems = nullptr;
        fst = rhs.fst;
        rhs.fst = nullptr;
        length = rhs.length;
        rhs.length = 0;
        currentSize = rhs.currentSize;
        rhs.currentSize = 0;
    }
}

MemoryStream& MemoryStream::operator=(MemoryStream&& rhs) noexcept{
    if(this != &rhs){
        close();
        dir = rhs.dir;
        rhs.dir = StreamDirection::None;
        mems = rhs.mems;
        rhs.mems = nullptr;
        fst = rhs.fst;
        rhs.fst = nullptr;
        length = rhs.length;
        rhs.length = 0;
        currentSize = rhs.currentSize;
        rhs.currentSize = 0;
    }
    return *this;
}

MemoryStream& MemoryStream::operator<<(MemoryStream& rhs) noexcept{
    if(dir == StreamDirection::None || rhs.dir == StreamDirection::None) return *this;

    if(dir == StreamDirection::Memory){
        if(!mems) return *this;
        reach(length + rhs.length);
        if(rhs.dir == StreamDirection::Memory){
            if(!rhs.mems) return *this;
            memcpy(mems + length, rhs.mems, rhs.length);
        }
        else{
            if(!rhs.fst) return *this;
            fread(mems + length, 1, rhs.length, rhs.fst);
        }
    }
    else{
        if(!fst) return *this;
        if(rhs.dir == StreamDirection::Memory){
            if(!rhs.mems) return *this;
            fwrite(rhs.mems, 1, rhs.length, fst);
        }
        else{
            if(!rhs.fst) return *this;
            rhs.open_file_as_buffer();

            fwrite(rhs.mems, 1, rhs.length, fst);

            rhs.close_file_as_buffer();
        }
    }
            
    return *this;
}

void MemoryStream::close() noexcept{
    switch(dir){
        case StreamDirection::None:
            return;
        case StreamDirection::Memory:
            dir = StreamDirection::None;
            if(!mems || !currentSize) return;
            free(mems);
            currentSize = 0;
            length = 0;
            mems = nullptr;
            return;
        case StreamDirection::File:
            dir = StreamDirection::None;
            if(!fst) return;
            fclose(fst);
            fst = nullptr;
            length = 0;
            currentSize = 0;
            return;
    }
}

void MemoryStream::open(const char* fileName, bool binary, bool append) noexcept{
    bin = binary;
    open_check();
    mems = nullptr;
    currentSize = 0;
    const char* mode = append ? "ab+" : "wb+";
    length = 0;
    fst = fopen(fileName, mode);
    if(!fst){
        std::cerr<<"File "<<fileName<<" not found"<<std::endl;
        dir = StreamDirection::None;
        return;
    }
    if(append){
        currentSize = ftell(fst);
        fseek(fst, 0, SEEK_END);
        length = ftell(fst);
        fseek(fst, currentSize, SEEK_SET);
    }
    dir = StreamDirection::File;
}

void MemoryStream::open(size_t size) noexcept{
    open_check();
    dir = StreamDirection::Memory;
    fst = nullptr;
    currentSize = (size + 15) & ~0xF;
    if(size == 0){
        mems = nullptr;
        dir = StreamDirection::None;
        return;
    }
    length = 0;
    mems = (char*)malloc(currentSize);
    if(!mems){
        dir = StreamDirection::None;
        currentSize = 0;
        return;
    }
}


}