#ifndef INERTIA_LEXERFILE_HPP
#define INERTIA_LEXERFILE_HPP

#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ios>
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

        LexerFileChunk() noexcept : view(){};
        LexerFileChunk(const char* data, size_t len, const LexerFile* _parent) noexcept : view(data, len), parent(_parent){};

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
        std::filesystem::path path;
        char* file = nullptr;
        size_t length : ((sizeof(size_t) * 8) - 1);
        bool to_free : 1;
        int err = 0;
        enum LexerFileErrCodes{
            FILE_NOT_FOUND = 1, ALLOC_ERROR = 2, READ_ERROR = 3
        };

        inline void clear() noexcept{
            path.clear();
            if(file && to_free){
                free(file);
            }
            file = nullptr;
            to_free = false;
            err = 0;
            length = 0;
        }

    public:
        LexerFile() noexcept : file(nullptr), length(0){}
        LexerFile(char* buff, size_t n, bool _to_free_ = false) noexcept{
            open(buff, n, _to_free_);
        }

        LexerFile(const std::filesystem::path& fileName) : file(nullptr), length(0){
            open(fileName);
        }

        ~LexerFile() noexcept{
            clear();
        }

        const std::filesystem::path filename() const noexcept{
            return path.filename();
        }

        const std::filesystem::path& get_path() const noexcept{
            return path;
        }

        std::pair<LexerFileChunk, LexerFileChunk> split(size_t at, bool* result = nullptr) const noexcept{
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

        LexerFile& operator=(LexerFile&& rhs) noexcept{
            if(this != &rhs){
                clear();
                file = rhs.file;
                length = rhs.length;
                rhs.file = nullptr;
                rhs.length = 0;
                
            }
            return *this;
        }

        void open(char* buff, size_t n, bool _free_ = false) noexcept{
            if(!buff || !n) return;
            clear();
            file = buff;
            length = n;
            to_free = _free_;
        }

        void open(const std::filesystem::path& fileName){
            clear();
            to_free = true;

            if(fileName.empty()){
                return;
            }
            path = fileName;

            std::ifstream ifstr(fileName, std::ios::binary | std::ios::ate);
            if(!ifstr.is_open()){
                err = LexerFileErrCodes::FILE_NOT_FOUND;
                length = 0;
                return;
            }

            length = ifstr.tellg();
            ifstr.seekg(0, std::ios::beg);

            file = (char*)malloc(length + 1);
            if(!file){
                ifstr.close();
                err = ALLOC_ERROR;
                length = 0;
                return;
            }

            if(!ifstr.read(file, length)){
                ifstr.close();
                err = READ_ERROR;
                free(file);
                file = nullptr;
                length = 0;
                return;
            }

            ifstr.close();
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
                throw std::out_of_range("Access in lexer file is out of range"); // was used for debugging
            }
            return *(file + i);
        }

        inline explicit operator bool() const noexcept{
            return file != nullptr;
        }

        friend std::ostream& operator<<(std::ostream& lhs, const LexerFile& rhs){
            if(!rhs) return lhs;
            size_t len = rhs.len();

            for(size_t i = 0; i < len; i++){
                lhs.put(rhs[i]);
            }
            return lhs;
        }
    };
}

#endif // INERTIA_LEXERFILE_HPP
