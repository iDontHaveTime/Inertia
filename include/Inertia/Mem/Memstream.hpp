#ifndef MEMSTREAM_HPP
#define MEMSTREAM_HPP

#include "Inertia/Mem/Archmem.hpp"
#include "Inertia/Mem/Autopointer.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

// UNFINISHED

namespace Inertia{
    enum class StreamDirection{
        None, File, Memory
    };
    class MemoryStream{
        StreamDirection dir = StreamDirection::None;
        size_t currentSize;
        size_t length;
        FILE* fst;
        char* mems;
        bool bin = false;
        Endian endianess = (Endian)0xFE;
        bool read_allowed = true;

        inline void double_size() noexcept{
            if(dir == StreamDirection::Memory){
                void* nrl = realloc(mems, currentSize<<1);
                if(!nrl){
                    return;
                }
                mems = (char*)nrl;
                currentSize <<= 1;
            }
            else return;
        }
        inline void open_check() noexcept{
            if((uint8_t)endianess == 0xFE){
                uint32_t num = 1;
                char* byte_ptr = (char*)(&num);
                if(byte_ptr[0] == 1){
                    endianess = Endian::IN_LITTLE_ENDIAN;
                }
                else{
                    endianess = Endian::IN_BIG_ENDIAN;
                }
            }
            if(dir != StreamDirection::None){
                close();
            }
        }
        inline void reach(size_t goal) noexcept{
            while(goal > currentSize){
                double_size();
            }
        }
        inline void open_file_as_buffer() noexcept{
            mems = (char*)malloc(length);
            if(!mems){
                return;
            }
            fread(mems, 1, length, fst);
        }
        inline void close_file_as_buffer() noexcept{
            if(mems && fst && dir == StreamDirection::File){
                free(mems);
                mems = nullptr;
            }
        }
    public:

        MemoryStream() : dir(StreamDirection::None), currentSize(0), length(0), fst(nullptr), mems(nullptr){};

        inline size_t size() const noexcept{
            if(*this){
                return length;
            }
            return 0;
        }

        inline void SetBinary(bool b) noexcept{
            bin = b;
        }

        inline size_t capacity() const noexcept{
            if(*this){
                if(dir == StreamDirection::File){
                    return length;
                }
                else{
                    return currentSize;
                }
            }
            return 0;
        }

        inline void open(void* memory, size_t size) noexcept{
            open_check();
            dir = StreamDirection::None;
            if(!memory || !size) return;
            dir = StreamDirection::Memory;
            mems = (char*)memory;
            currentSize = size;
            length = 0;
        }

        void open(size_t size) noexcept;

        inline void open(FILE* f, bool allow_read){
            if(!f) return;
            if(f == stdin) return;
            open_check();
            dir = StreamDirection::None;
            read_allowed = allow_read;
            if(f == stderr || f == stdout){
                read_allowed = false;
            }
            fst = f;
            dir = StreamDirection::File;
            length = ftell(f);
            currentSize = 0;
            mems = nullptr;
        }
        MemoryStream(FILE* f, bool allow_read) noexcept{
            open(f, allow_read);
        }

        // Open a simple memory buffer
        inline void open() noexcept{
            open_check();
            open(16);
        }

        MemoryStream(void* memory, size_t size) noexcept{
            open(memory, size);
        }

        MemoryStream(size_t size) noexcept{
            open(size);
        }

        inline void read_endian(void* dest, size_t offset = 0) noexcept{
            if(!read_allowed) return;
            if(offset >= length) return;
            read_off(dest, offset);
            if(endianess != MachineEndian){
                FlipMemEndian(dest, length - offset);
            }
        }

        inline void read_off(void* dest, size_t offset = 0) noexcept{
            if(!read_allowed) return;
            if(offset >= length) return;
            if(dir == StreamDirection::Memory){
                if(!mems) return;
                memcpy(dest, mems + offset, length - offset);
            }
            else if(dir == StreamDirection::File){
                if(!fst) return;
                currentSize = ftell(fst);
                fseek(fst, offset, SEEK_SET);
                fread(dest, 1, length - offset, fst);
                fseek(fst, currentSize, SEEK_SET);
            }
        }

        void open(const char* fileName, bool binary = false, bool append = false) noexcept;

        MemoryStream(const char* fileName, bool binary = false, bool append = false) noexcept{
            open(fileName, binary, append);
        }

        void close() noexcept;

        ~MemoryStream() noexcept{
            close();
        }

        operator bool() const noexcept{
            return dir != StreamDirection::None;
        }

        inline void SetEndianess(Endian endian) noexcept{
            endianess = endian;
        }

        inline const char* data() const noexcept{
            if(dir == StreamDirection::File){
                // uhm?
                return nullptr;
            }
            else if(dir == StreamDirection::Memory){
                return mems;
            }
            return nullptr;
        }

        template<typename T>
        inline void write_endian(const T& _mem) noexcept{
            if(MachineEndian == endianess){
                write(&_mem, sizeof(T));
            }
            if(endianess != MachineEndian){
                uint8_t flipped[sizeof(T)];
                uint16_t fpi = 0;
                for(size_t i = sizeof(T); i > 0; i--){
                    flipped[fpi] = ((uint8_t*)&_mem)[i - 1];
                    fpi++;
                }
                write(flipped, sizeof(T));
            }
        }

        inline void write(const void* _mem, size_t _size) noexcept{
            if(!_mem || !_size || dir == StreamDirection::None) return;

            if(dir == StreamDirection::Memory){
                reach(length + _size);
                std::memcpy(mems + length, _mem, _size);
                length += _size;
            }
            else{
                if(fst){
                    fwrite(_mem, 1, _size, fst);
                    length += _size;
                }
            }
        }

        inline void flush() noexcept{
            if(dir == StreamDirection::File){
                fflush(fst);
            }
        }

        template<typename T>
        inline void write(const T& _mem) noexcept{
            write(&_mem, sizeof(T));
        }

        inline void putb(int c) noexcept{
            if(dir == StreamDirection::File){
                if(!fst) return;
                putc(c, fst);
            }
            else if(dir == StreamDirection::Memory){
                if(!mems) return;
                if(length + 1 >= currentSize){
                    double_size();
                }
                mems[length++] = (char)c;
            }
        }

        inline void seek(size_t pos) noexcept{
            if(dir == StreamDirection::None) return;
            if(dir == StreamDirection::Memory){
                if(!mems) return;
            }
            else{
                if(!fst) return;
                fseek(fst, pos, SEEK_SET);
            }
            length = pos;
        }
        
        MemoryStream& operator<<(MemoryStream& rhs) noexcept;

        MemoryStream& operator=(MemoryStream&& rhs) noexcept;
        MemoryStream(MemoryStream&& rhs) noexcept;

        MemoryStream& operator<<(char c) noexcept{
            if(dir == StreamDirection::None) return *this;
            
            if(dir == StreamDirection::Memory){
                if(!mems) return *this;
                if(length + 1 > currentSize){
                    double_size();
                }
                mems[length++] = c;
            }
            else{
                if(!fst) return *this;
                putc(c, fst);
                length++;
            }
            return *this;
        }

        MemoryStream& operator<<(const char* s) noexcept{
            write(s, strlen(s));
            return *this;
        }

        MemoryStream& operator<<(int n) noexcept{
            if(bin){
                write(&n, sizeof(int));
            }
            else{
                char buff[32];
                sprintf(buff, "%d", n);
                write(buff, strlen(buff));
            }
            return *this;
        }

        MemoryStream& operator<<(double n) noexcept;

        MemoryStream& operator<<(std::string s){
            write(s.data(), s.size());
            return *this;
        }

        MemoryStream& operator<<(std::string_view s){
            write(s.data(), s.size());
            return *this;
        }

        AutoPointer<char, AutoPointerType::Array> to_buffer();

        inline void clear() noexcept{
            if(dir == StreamDirection::File){
                freopen(nullptr, "w", fst);
            }
            length = 0;
        }

        friend std::ostream& operator<<(std::ostream& lhs, MemoryStream& rhs){
            if(rhs.dir == StreamDirection::None) return lhs;

            if(rhs.dir == StreamDirection::Memory){
                if(!rhs.mems) return lhs;
                lhs.write(rhs.mems, rhs.length);
            }
            else{
                if(!rhs.fst) return lhs;
                rhs.open_file_as_buffer();

                lhs.write(rhs.mems, rhs.length);

                rhs.close_file_as_buffer();
            }
            
            return lhs;
        }

        inline bool read(void* _dest, size_t n = 0){
            if(!read_allowed) return false;
            if(dir == StreamDirection::None) return false;
            if(!n) n = length;

            if(dir == StreamDirection::File){
                if(!fst) return false;
                currentSize = ftell(fst);
                fseek(fst, 0, SEEK_SET);
                fread(_dest, 1, n, fst);
                fseek(fst, currentSize, SEEK_SET);
            }
            else{
                if(!mems) return false;
                memcpy(_dest, mems, n);
            }

            return true;
        }

        inline void dump(FILE* dest) noexcept{
            if(dir == StreamDirection::None) return;

            if(dir == StreamDirection::File){
                if(!fst) return;
                open_file_as_buffer();

                fwrite(mems, 1, length, dest);

                close_file_as_buffer();
            }
            else{
                if(!mems) return;
                fwrite(mems, 1, length, dest);
            }
        }
    };
};

#endif // MEMSTREAM_HPP
