#ifndef INERTIA_SUPPORT_STREAM_H
#define INERTIA_SUPPORT_STREAM_H

#include <inr/ADT/FileHandle.h>
#include <inr/Platform/Unified.h>
#include <inr/Support/Builtin.h>
#include <cstdlib>
#include <cstring>
#include <concepts>
#include <charconv>
#include <limits>

namespace inr{

    /**
     * @brief An output stream to a provided ufile.
     */
    class ostream{
    public:
        using manipulator = ostream&(*const)(ostream&);
        using char_type = char;

        constexpr static size_t starting_buffer = 0x2000;

    private:
        fs::ufile file;
        char_type* buf_start, *buf_end, *buf_cur;
        enum class Buffering{
            INTERNAL, EXTERNAL, NONE
        } buf;
    protected:


    public:
        const fs::ufile& get_file() const noexcept{
            return file;
        }

        ostream() = delete;

        void flush() noexcept{
            if(buf == Buffering::NONE || !file.valid()){
                return;
            }

            if(buf_start == buf_cur) return;

            uwrite(buf_start, sizeof(char_type), size_t(buf_cur - buf_start), file);
            buf_cur = buf_start;
        }

        void RemoveBuffer() noexcept{
            flush();
            if(buf == Buffering::INTERNAL) std::free(buf_start);
            buf_start = buf_cur = buf_end = nullptr;
            buf = Buffering::NONE;
        }

        void SetBuffer(size_t size) noexcept{
            if(size == 0){
                RemoveBuffer();
                return;
            }

            if(buf != Buffering::EXTERNAL && size == size_t(buf_end - buf_start)){
                return;
            }

            flush();

            if(buf == Buffering::EXTERNAL){
                RemoveBuffer();
            }

            void* bs = std::realloc(buf_start, size);
            if(bs){
                buf_cur = buf_start = (char_type*)bs;
                buf_end = buf_start + size;
                buf = Buffering::INTERNAL;
            }
            else{
                RemoveBuffer();
            }
        }

        void SetBuffer(char_type* a, char_type* b) noexcept{
            RemoveBuffer();
            buf_cur = buf_start = a;
            buf_end = b;
            buf = Buffering::EXTERNAL;
        }

        ostream(const fs::ufile& f, bool buffered = false, char_type* _buf_start = nullptr,
            char_type* _buf_end = nullptr) noexcept : file(f){
            if(buffered){
                if(_buf_start && _buf_end){
                    SetBuffer(_buf_start, _buf_end);
                }
                else{
                    SetBuffer(starting_buffer);
                }
            }
            else{
                SetBuffer(0);
            }
        }

        ostream& write(const char_type* s, size_t c) noexcept{
            if(!file.valid()) return *this;

            if(buf == Buffering::NONE || c >= size_t(buf_end - buf_start)){
                uwrite((const void*)s, sizeof(char_type), c, file);
            }
            else{
                if(buf_cur + c >= buf_end){
                    flush();
                }
                inr::memcpy(buf_cur, s, c);
                buf_cur += c;
            }
            
            return *this;
        }


        ~ostream() noexcept{
            RemoveBuffer();
        }

        size_t get_buffer_size() const noexcept{
            if(buf == Buffering::NONE) return 0;
            return size_t(buf_end - buf_start);
        }

        size_t get_buffer_index() const noexcept{
            if(buf == Buffering::NONE) return 0;
            return size_t(buf_cur - buf_start);
        }

        ostream& operator<<(const char_type* c_type) noexcept{
            size_t bytes = inr::strlen(c_type);

            if constexpr(sizeof(char_type) > 1){
                bytes /= sizeof(char_type);
            }

            return write(c_type, bytes);
        }

        ostream& operator<<(char_type c) noexcept{
            return write(&c, 1);
        }

        ostream& operator<<(manipulator mp){
            return mp(*this);
        }

        ostream& operator<<(bool b) noexcept{
            return (*this)<<(b ? "true" : "false");
        }

        template<std::integral T>
        ostream& operator<<(T n) noexcept{
            char_type buff[std::numeric_limits<T>::digits10 + 2];
            std::to_chars_result res = std::to_chars(buff, buff + sizeof(buff), n);

            if(res.ec == std::errc()){
                write(buff, res.ptr - buff);
            }
            else{
                (*this)<<"err";
            }
            return *this;
        }
    };

    extern ostream out;
    extern ostream err;
    extern ostream::manipulator endl;
    extern ostream::manipulator flush;
}

#endif // INERTIA_SUPPORT_STREAM_H
