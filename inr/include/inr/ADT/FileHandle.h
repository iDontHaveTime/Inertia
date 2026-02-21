#ifndef INERTIA_ADT_FILEHANDLE_H
#define INERTIA_ADT_FILEHANDLE_H

#include <cstdint>
#include <cstdio>

namespace inr::fs{
    
    /**
     * @brief A file handle that can handle all file handle types.
     *
     * This file handle can handle POSIX's (or others) fd (int),
     * Windows' HANDLE (void*),
     * or just fallback to standard's FILE.
     */
    class ufile{
    public:
        using posix_type = int;
        using standard_type = FILE*;
        using windows_type = void*;
        enum class handle_type : uint8_t{
            POSIX, WINDOWS, STANDARD
        };
    private:
        union{
            posix_type fd;
            standard_type standard;
            windows_type handle;
        };
        handle_type type;
    public:
        ufile() = delete;

        ufile(const ufile&) noexcept = default;
        ufile(ufile&&) noexcept = default;

        ufile& operator=(const ufile&) noexcept = default;
        ufile& operator=(ufile&&) noexcept = default;

        ufile(handle_type _type) noexcept : type(_type){
            invalidate();
        }

        ufile(posix_type _fd) noexcept : fd(_fd), type(handle_type::POSIX){};
        ufile(standard_type _standard) noexcept : standard(_standard), type(handle_type::STANDARD){};
        ufile(windows_type _handle) noexcept : handle(_handle), type(handle_type::WINDOWS){};

        posix_type asPOSIX() const noexcept{
            return fd;
        }

        standard_type asFILE() const noexcept{
            return standard;
        }

        windows_type asHANDLE() const noexcept{
            return handle;
        }

        handle_type get_type() const noexcept{
            return type;
        }

        bool valid() const noexcept{
            switch(type){
                case handle_type::POSIX: return fd >= 0;
                case handle_type::WINDOWS: return handle != nullptr;
                case handle_type::STANDARD: return standard != nullptr;
                default: return false;
            }
        }

        void invalidate() noexcept{
            switch(type){
                case handle_type::POSIX:
                    fd = -1;
                    break;
                case handle_type::WINDOWS:
                    handle = nullptr;
                    break;
                case handle_type::STANDARD:
                    [[fallthrough]];
                default:
                    standard = nullptr;
                    break;
            }
        }

        ~ufile() noexcept = default;
    };

}

#endif // INERTIA_ADT_FILEHANDLE_H
