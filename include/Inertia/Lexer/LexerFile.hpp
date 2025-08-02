#ifndef INERTIA_LEXERFILE_HPP
#define INERTIA_LEXERFILE_HPP

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <ostream>
#include <utility>

namespace Inertia{
    static const char* LexerFileErrStr[] = {
        "No errors", "File not found", "Allocation error", "Read error"
    };
    class LexerFile;
    struct LexerFileChunk{
        std::string_view view;
        const LexerFile* parent;

        LexerFileChunk() : view(){};
        LexerFileChunk(const char* data, size_t len, const LexerFile* _parent) : view(data, len), parent(_parent){};

        char operator[](size_t i) const{
            if(i >= view.length()){
                throw std::out_of_range("Access in lexer file chunk is out of range");
            }
            return *(view.data() + i);
        }

        inline const char* raw() const noexcept{
            return view.data();
        }
        inline size_t len() const noexcept{
            return view.length();
        }

        friend std::ostream& operator<<(std::ostream& lhs, LexerFileChunk& rhs){
            if(rhs.view.empty()) return lhs;
            std::size_t len = rhs.len();

            for(std::size_t i = 0; i < len; i++){
                lhs.put(rhs[i]);
            }
            return lhs;
        }
    };
    class LexerFile{
        const char* fname = nullptr;
        char* file = nullptr;
        std::size_t length;
        int err = 0;
        enum LexerFileErrCodes{
            FILE_NOT_FOUND = 1, ALLOC_ERROR = 2, READ_ERROR = 3
        };

        inline void clear() noexcept{
            fname = nullptr;
            if(file){
                std::free(file);
                file = nullptr;
            }
            err = 0;
            length = 0;
        }

    public:
        LexerFile() noexcept : file(nullptr), length(0){}

        LexerFile(const char* fileName) noexcept : file(nullptr), length(0){
            open(fileName);
        }

        ~LexerFile() noexcept{
            clear();
        }

        const char* filename() const noexcept{
            return fname;
        }

        std::pair<LexerFileChunk, LexerFileChunk> split(size_t at, bool* result = nullptr) const{
            if(!file || length <= 1){
                if(result) *result = false;
                std::cerr<<"Splitting file into chunks failed"<<std::endl;
                return {};
            }

            if(at > length){
                if(result) *result = false;
                std::cerr<<"Split position exceeds file length"<<std::endl;
                return {};
            }
            
            if(result) *result = true;
            LexerFileChunk p1(file, at, this);
            LexerFileChunk p2(file + at, length - at, this);

            return {p1, p2};
        }

        inline const char* fend() const noexcept{
            if(!file) return nullptr;
            return (file + length);
        }

        LexerFile(const LexerFile&) = delete;
        LexerFile& operator=(const LexerFile&) = delete;

        LexerFile& operator=(LexerFile&& rhs){
            if(this != &rhs){
                clear();
                file = rhs.file;
                length = rhs.length;
                rhs.file = nullptr;
                rhs.length = 0;
                
            }
            return *this;
        }

        void open(const char* fileName){
            clear();

            if(!fileName){
                return;
            }
            fname = fileName;

            std::FILE* f = std::fopen(fileName, "rb");
            if(!f){
                err = FILE_NOT_FOUND;
                return;
            }

            std::fseek(f, 0, SEEK_END);
            length = std::ftell(f);
            std::rewind(f);

            file = (char*)std::malloc(length + 1);
            if(!file){
                std::fclose(f);
                err = ALLOC_ERROR;
                length = 0;
                return;
            }

            if(std::fread(file, 1, length, f) != length){
                err = READ_ERROR;
                std::free(file);
                file = nullptr;
                length = 0;
                std::fclose(f);
                return;
            }

            std::fclose(f);
            *(file + length) = '\0';
        }

        inline const char* raw() const noexcept{
            return file;
        }

        inline std::size_t len() const noexcept{
            return length;
        }

        char operator[](size_t i) const{
            if(i >= length){
                throw std::out_of_range("Access in lexer file is out of range");
            }
            return *(file + i);
        }

        inline explicit operator bool() const noexcept{
            return file != nullptr;
        }

        friend std::ostream& operator<<(std::ostream& lhs, const LexerFile& rhs){
            if(!rhs) return lhs;
            std::size_t len = rhs.len();

            for(std::size_t i = 0; i < len; i++){
                lhs.put(rhs[i]);
            }
            return lhs;
        }

        // open from memory, dont add a +1 for null terminator
        inline void open(const void* mem, size_t size){
            clear();
            if(*((char*)mem + size - 1) == '\0'){
                size--;
            }
            length = size;
            file = (char*)malloc(size + 1);
            if(!file){
                err = ALLOC_ERROR;
                length = 0;
                return;
            }

            memcpy(file, mem, length);

            *((char*)file + length) = '\0';
        }

        // uses strlen, null termination needed
        inline void open_str(const char* str){
            clear();
            length = strnlen(str, INTMAX_MAX);

            file = (char*)malloc(length + 1);
            if(!file){
                err = ALLOC_ERROR;
                length = 0;
                return;
            }

            memcpy(file, str, length);

            *((char*)file + length) = '\0';
        }
    };
}

#endif // INERTIA_LEXERFILE_HPP
