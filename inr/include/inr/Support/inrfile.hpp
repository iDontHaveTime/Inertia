#ifndef INERTIA_INRFILE_HPP
#define INERTIA_INRFILE_HPP

#include "inr/Defines/inrapis.hpp"
#include "inr/Defines/inrfiledef.hpp"
#include "inr/Support/inrbuf.hpp"
#include "inr/Support/inriterator.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>

/**
 * @file inr/Support/inrfile.hpp
 * @brief Inertia's implementation of files.
 *
 * This header contains Inertia's implementation of a file.
 * Its aimed to optimize files as much as possible, using OS native stuff when possible.
 *
 **/

namespace inr{

    /**
     * @brief Inertia's in-memory file.
     *
     * 
     */
    class inr_mem_file{
        void* file;
        using mem_file_close = void (*)(inr_mem_file&);
        size_t _size;
        mem_file_close closer;
        enum{
            ERROR_ALLOC, ERROR_SEEK, ERROR_FILE_OPENED_WRONG
        };
        uint32_t errc;

        inr_mem_file(void* _file, mem_file_close fptr, size_t sz) noexcept : file(_file), _size(sz), closer(fptr), errc(0){};
        inr_mem_file(uint32_t errs = 0) noexcept : file(nullptr), _size(0), closer(nullptr), errc(errs){};
    public:

        /**
         * @brief Gets the underlying memory file handle, const version.
         *
         * @return Const pointer to the file.
         */
        const void* get_file() const noexcept{
            return file;
        }

        /**
         * @brief Gets the underlying memory file handle.
         *
         * @return Pointer to the file.
         */
        void* get_file() noexcept{
            return file;
        }

        /**
         * @brief Size in bytes of the file.
         *
         * @return Size of the file.
         */
        size_t size() const noexcept{
            return _size;
        }

        /**
         * @brief Error code, defined in 'inr_mem_file' with the prefix ERROR_*.
         *
         * @return Error code.
         */
        uint32_t get_errc() const noexcept{
            return errc;
        }

        inr_mem_file(const inr_mem_file&) = delete;
        inr_mem_file& operator=(const inr_mem_file&) = delete;

        inr_mem_file(inr_mem_file&& other) noexcept{
            file = other.file;
            other.file = nullptr;
            _size = other._size;
            other._size = 0;
            closer = other.closer;
            other.closer = nullptr;
        }

        inr_mem_file& operator=(inr_mem_file&& other) noexcept{
            if(this != &other){
                file = other.file;
                other.file = nullptr;
                _size = other._size;
                other._size = 0;
                closer = other.closer;
                other.closer = nullptr;
            }
            return *this;
        }

        /**
         * @brief Frees the memory of the mem file.
         */
        void close() noexcept{
            if(!file) return;
            if(closer){
                closer(*this);
            }
        }

        /**
         * @brief Returns the validity of the pointer.
         *
         * @return True if everything is correct, false if something is not.
         */
        bool valid() const noexcept{
            return file && closer;
        }

        operator bool() const noexcept{
            return valid();
        }

        ~inr_mem_file() noexcept{
            close();
        }

        friend class inrfile;

        friend class inr_posix_handle;
        friend class inr_windows_handle;

        friend struct inr_file_handle;

        array_iterator<uint8_t> begin() noexcept{
            return array_iterator{(uint8_t*)file};
        }
        array_iterator<uint8_t> end() noexcept{
            return array_iterator{(uint8_t*)file + size()};
        }

        array_iterator<const uint8_t> begin() const noexcept{
            return array_iterator{(const uint8_t*)file};
        }
        array_iterator<const uint8_t> end() const noexcept{
            return array_iterator{(const uint8_t*)file + size()};
        }

        array_iterator<const uint8_t> cbegin() const noexcept{
            return array_iterator{(const uint8_t*)file};
        }
        array_iterator<const uint8_t> cend() const noexcept{
            return array_iterator{(const uint8_t*)file + size()};
        }

        uint8_t& operator[](size_t n){
            return ((uint8_t*)file)[n];
        }
    };

    /**
     * @brief Inertia's POSIX handle.
     */
    class inr_posix_handle{
        int fd;
        inrbuf<char> buff;

        static inr_posix_handle* inr_new_posix_handle(int _fd, size_t buf_size);
        static void inr_delete_posix_handle(inr_posix_handle* _posix);
        static inr_mem_file inr_posix_memfile(const inr_posix_handle* _posix);

        long write(const void* data, size_t n);
        int flush();
        void close();
    public:

        inr_posix_handle(int _fd, char* start, size_t size) noexcept : fd(_fd), buff(start, size){};

        bool valid() const noexcept{
            return fd >= 0;
        }

        friend struct inr_file_handle;
    };

    /**
     * @brief Inertia's Windows handle.
     */
    class inr_windows_handle{
        void* handle;
        inrbuf<char> buff;

        static inr_windows_handle* inr_new_windows_handle(void* _handle, size_t buf_size);
        static void inr_delete_windows_handle(inr_windows_handle* _win);
        static inr_mem_file inr_windows_memfile(const inr_windows_handle* _win);
        long write(const void* data, size_t n);
        int flush();
        void close();
    public:

        inr_windows_handle(void* _handle, char* start, size_t size) noexcept : handle(_handle), buff(start, size){};

        bool valid() const noexcept{
            return handle != nullptr;
        }

        friend struct inr_file_handle;
    };
    
    /**
     * @brief The file handle the 'inrfile' class uses.
     *
     * This class holds POSIX's fd (int), Windows' HANDLE (void*), and Standard's file (FILE*). 
     * On x86-64 (64bit):
     *  union - 8 bytes
     *  meta - 4 bytes
     *  api - 1 byte
     *  opt - 1 byte
     *  ownership - 1 byte
     *  last_op - 1 byte
     * Total: 16. Alignment: 8 bytes.
     *
     * On i386 (32bit):
     *  union - 4 bytes
     *  meta - 4 bytes
     *  api - 1 byte
     *  opt - 1 byte
     *  ownership - 1 byte
     *  last_op - 1 byte
     * Total: 12. Alignment: 4 bytes.
     */
    struct inr_file_handle{
    private:
        /**
         * @brief Union to hold every type of file (every mainstream type).
         */
        union{
            /**
             * @brief POSIX's file descriptor handle (int).
             */
            inr_posix_handle* fd;
            /**
             * @brief Standard library's FILE.
             */
            FILE* file;
            /**
             * @brief Windows' HANDLE (void*).
             */
            inr_windows_handle* handle;
        };
        /**
         * @brief Extra metadata, not used by the file handle.
         */
        uint32_t meta = 0;
        /**
         * @brief The underlying API of the file handle.
         */
        APIs api;
        /**
         * @brief How was the file opened.
         */
        fs::OpeningType opt;
        /**
         * @brief Should the file handle close if called.
         */
        bool ownership;
        /**
         * @brief No read after write or vice versa, must flush before doing one after another.
         *
         * The C standard mentions that files that have been opened with Read and Write ("w+" or "r+")
         * must flush or seek to reset the internal file buffer before doing the opposite operation.
         * And to comply you should do the same here, no matter if its standard, posix, or windows APIs.
         */
        fs::LastFileOperation last_op = fs::LastFileOperation::None;

        inr_file_handle() noexcept : file(nullptr), api(APIs::NONE), opt(fs::OpeningType::None), ownership(false){};
        explicit inr_file_handle(FILE* _file, fs::OpeningType _open, bool own) noexcept : file(_file), api(APIs::STANDARD), opt(_open), ownership(own){};
        explicit inr_file_handle(int _fd, size_t buffer_size, fs::OpeningType _open, bool own) noexcept : fd(inr_posix_handle::inr_new_posix_handle(_fd, buffer_size)), api(APIs::POSIX), opt(_open), ownership(own){};
        explicit inr_file_handle(void* _handle, size_t buffer_size, fs::OpeningType _open, bool own) noexcept : handle(inr_windows_handle::inr_new_windows_handle(_handle, buffer_size)), api(APIs::WINDOWS), opt(_open), ownership(own){};

        // explained in 'inrfile'.
        long write(const void* data, size_t size, size_t n) noexcept;
        // explained in 'inrfile'.
        int read(void* dest, size_t size, size_t n) noexcept;

        // explained in 'inrfile'
        inr_mem_file fmem_open() const;

        int flush() noexcept;

        void close() noexcept;

        void copy(const inr_file_handle& other) noexcept{
            switch(other.api){
                case APIs::POSIX:
                    fd = other.fd;
                    break;
                case APIs::WINDOWS:
                    handle = other.handle;
                    break;
                case APIs::STANDARD:
                    file = other.file;
                    break;
                default:
                    break;
            }
        }

        inr_file_handle(const inr_file_handle& other) = delete;
        inr_file_handle& operator=(const inr_file_handle& other) = delete;

        inr_file_handle(inr_file_handle&& other) noexcept{
            copy(other);
            other.file = nullptr;

            api = other.api;
            other.api = APIs::NONE;

            opt = other.opt;
            other.opt = fs::OpeningType::None;

            ownership = other.ownership;
            other.ownership = false;
        }
        inr_file_handle& operator=(inr_file_handle&& other) noexcept{
            if(this != &other){
                copy(other);
                other.file = nullptr;
    
                api = other.api;
                other.api = APIs::NONE;
    
                opt = other.opt;
                other.opt = fs::OpeningType::None;
    
                ownership = other.ownership;
                other.ownership = false;
            }
            return *this;
        }

        uint32_t& access_meta() noexcept{
            return meta;
        }

        const uint32_t& access_meta() const noexcept{
            return meta;
        }

        ~inr_file_handle() noexcept = default;
    public:
        /**
         * @brief Checks if the internal handle is valid.
         */
        bool valid() const noexcept{
            switch(api){
                case APIs::POSIX:
                    return fd && fd->valid();
                case APIs::WINDOWS:
                    return handle && handle->valid();
                case APIs::STANDARD:
                    return file != nullptr;
                default:
                    return false;
            }
        }

        friend class inrfile;
    };

    const char* inr_open_type_to_cfopen(fs::OpeningType opt) noexcept;

    /**
     * @brief Inertia's file handle wrapper.
     * This is a super lightweight file wrapper, provides simple writing and reading functions. 
     */
    class inrfile{
        /// The underlying handle.
        inr_file_handle file;
    public:

        /**
         * @brief Makes a null file. 
         */
        inrfile() noexcept : file(){};

        /**
         * @brief Opens the provided file handle instead of the current one.
         *
         * This function opens the new file, and closes the last one if it had the ownership of it. 
         *
         * @param _file The file to grab.
         * @param open_type How is the file opened.
         * @param owned Should this class own the file.
         */
        void open(FILE* _file, fs::OpeningType open_type, bool owned) noexcept{
            close();
            file = inr_file_handle{_file, open_type, owned};
        }

        /**
         * @brief Opens the provided file descriptor instead of the current one.
         *
         * This function opens the new file, and closes the last one if it had the ownership of it. 
         *
         * @param fd File descriptor.
         * @param open_type How is the file opened.
         * @param owned Should this class own the file.
         */
        void open(int fd, size_t buffer_size, fs::OpeningType open_type, bool owned) noexcept{
            close();
            file = inr_file_handle{fd, buffer_size, open_type, owned};
        }

        /**
         * @brief Grabs the provided file and chooses ownership.
         * 
         * This constructor is the already-existing file constructor.
         *
         * @param _file The file to grab.
         * @param open_type How is the file opened (reading or writing).
         * @param owning Should this class own the file.
         */
        inrfile(FILE* _file, fs::OpeningType open_type, bool owning) noexcept{
            open(_file, open_type, owning);
        };

        /**
         * @brief Grabs the provided file descriptor and chooses ownership.
         * 
         * This constructor is the already-existing file descriptor constructor.
         *
         * @param fd The file descriptor.
         * @param open_type How is the file opened (reading or writing).
         * @param owning Should this class own the file.
         */
        inrfile(int fd, size_t buffer_size, fs::OpeningType open_type, bool owning) noexcept{
            open(fd, buffer_size, open_type, owning);
        }

        /**
         * @brief Returns the underlying file (const version).
         * @return The underlying const FILE handle.
         */
        const inr_file_handle& get_file() const noexcept{
            return file;
        }
        /**
         * @brief Returns the underlying file.
         * @return The underlying FILE handle.
         */
        inr_file_handle& get_file() noexcept{
            return file;
        }

        /**
         * @brief Flushes the file.
         * @return EOF if error.
         */
        int flush() noexcept{
            return file.flush();
        }

        /**
         * @brief Writes to the file provided.
         *
         * @param data The data to write into the file.
         * @param size Size of the object to write.
         * @param n The number of objects to write.
         *
         * @return Amount of objects written, if error returns EOF. 
         */
        long write(const void* data, size_t size, size_t n) noexcept{
            return file.write(data, size, n);
        }

        /**
         * @brief Reads from file to the buffer provided.
         *
         * @param dest The destination to copy the data to.
         * @param size Size of the object to read.
         * @param n The number of objects to read.
         *
         *
         * @return if error returns EOF. 
         */
        int read(void* dest, size_t size, size_t n) noexcept{
            return file.read(dest, size, n);
        }

        /**
         * @brief Returns if the file is owned by this class.
         * @return Ownership of the file.
         */
        bool owning() const noexcept{
            return file.ownership;
        }

        /**
         * @brief Checks if the underlying file is valid.
         * @return True if file is not nullptr, false if it is nullptr.
         */
        operator bool() const noexcept{
            return valid();
        }

        /**
         * @brief Checks if the underlying file is valid.
         * @return True if file is not nullptr, false if it is nullptr.
         */
        bool valid() const noexcept{
            return file.valid();
        }

        /**
         * @brief Closes the underlying file if owned, otherwise nulls it.
         */
        void close() noexcept{
            file.close();
            file = inr_file_handle{};
        }

        /**
         * @brief Closes the file if owned.
         */
        ~inrfile() noexcept{
            close();
        }

        /**
         * @brief The underlying file handle allows to store extra metadata.
         * @return A read/write reference to metadata.
         */
        uint32_t& access_meta() noexcept{
            return file.access_meta();
        }

        /**
         * @brief The underlying file handle allows to store extra metadata.
         * @return A view const reference to metadata.
         */
        const uint32_t& access_meta() const noexcept{
            return file.access_meta();
        }

        /**
         * @brief Opens the file in memory.
         *
         * Uses mmap in POSIX and malloc + fread on standard.
         */
        inr_mem_file fmem_open() const{
            if(!((uint8_t)file.opt & 0b1000000)){
                return {inr_mem_file::ERROR_FILE_OPENED_WRONG};
            }
            return file.fmem_open();
        }

        inrfile(const inrfile&) noexcept = delete;
        inrfile& operator=(const inrfile&) noexcept = delete;

        inrfile(inrfile&&) noexcept = default;
        inrfile& operator=(inrfile&&) noexcept = default;
    };
}

#endif // INERTIA_INRFILE_HPP
