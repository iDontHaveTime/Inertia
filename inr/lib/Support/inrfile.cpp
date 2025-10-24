/* Inertia's includes. */
#include "inr/Support/inrfile.hpp"
#include "inr/Defines/inrapis.hpp"
#include "inr/Defines/inrfiledef.hpp"

/* C standard includes. */
#include <cstdio>
#include <cstdlib>

/* C++ STL includes. */
#include <new>

/* POSIX includes.*/
#ifdef INERTIA_POSIX
#include <unistd.h>
#include <sys/mman.h>
#endif

/* Enter the namespace. */
namespace inr{

/* API specific write. */
write_integer inr_file_handle::write(const void* data, size_t size, size_t n) noexcept{
    /* Do not write if the file isn't valid and not writeable. */
    if(valid() && fs::opening_type_write(opt)){
        /* Return 0 if the last operation was reading. */
        if(last_op == fs::LastFileOperation::Reading) return 0;
        /* Set the last operation (this one) to write. */
        last_op = fs::LastFileOperation::Writing;
        switch(api){
            /* In case of a POSIX API use the fd's write. */
            case APIs::POSIX:{
                    write_integer res = fd->write(data, size * n);
                    return res == 0 ? 0 : res / size;
                }
            /* In case of a Windows API use the HANDLE's write. */
            case APIs::WINDOWS:{
                    write_integer res = handle->write(data, size * n);
                    return res == 0 ? 0 : res / size;
                }
            /* Use the C standard function for standard files. */
            case APIs::STANDARD:
                return fwrite(data, size, n, file);
            /* How does one even get here. */
            default:
                return 0;
        }
    }
    else{
        /* Isn't valid or open type wasn't writeable. */
        return 0;
    }
}

/* Implement general seek. */
seek_integer inr_file_handle::seek(tell_integer off, fs::SeekType st) noexcept{
    if(valid()){
        if(last_op == fs::LastFileOperation::Writing){
            if(flush() == EOF) return EOF;
        }
        else if(last_op == fs::LastFileOperation::Reading){
            switch(api){
                case APIs::POSIX:
                    fd->buff.flush();
                    fd->read_i = 0;
                    break;
                case APIs::WINDOWS:
                    handle->buff.flush();
                    handle->read_i = 0;
                    break;
                default:
                    break;
            }
        }
        last_op = fs::LastFileOperation::None;
        switch(api){
            case APIs::POSIX:
                return fd->seek(off, st);
            case APIs::WINDOWS:
                return handle->seek(off, st);
            case APIs::STANDARD:
                return fseek(file, off, (int)st);
            default:
                return EOF;
        }
    }
    else{
        return EOF;
    }
}

/* Implement general tell. */
tell_integer inr_file_handle::tell() noexcept{
    if(valid()){
        switch(api){
            case APIs::POSIX:
                return fd->tell(*this);
            case APIs::WINDOWS:
                return handle->tell(*this);
            case APIs::STANDARD:
                return ftell(file);
            default:
                return EOF;
        }
    }
    else{
        return EOF;
    }
}

/* Implement general flush. */
int inr_file_handle::flush() noexcept{
    if(valid() && fs::opening_type_write(opt)){
        last_op = fs::LastFileOperation::None;
        switch(api){
            case APIs::POSIX:
                return fd->flush();
            case APIs::WINDOWS:
                return handle->flush();
            case APIs::STANDARD:
                return fflush(file);
            default:
                return EOF;
        }
    }
    else{
        return EOF;
    }
}

/* Implement general close. */
void inr_file_handle::close() noexcept{
    if(api == APIs::POSIX && fd){
        flush();

        if(ownership){
            fd->close();
        }

        inr_posix_handle::inr_delete_posix_handle(fd);
        fd = nullptr;
    }
    else if(api == APIs::WINDOWS && handle){
        flush();

        if(ownership){
            handle->close();
        }

        inr_windows_handle::inr_delete_windows_handle(handle);
        handle = nullptr;
    }
    else if(api == APIs::STANDARD && file){
        flush();

        if(ownership){
            fclose(file);
        }

        file = nullptr;
    }

    ownership = false;
    api = APIs::NONE;
}

/* Translates Inertia's open type to fopen's mode. */
const char* inr_open_type_to_cfopen(fs::OpeningType opt) noexcept{
    switch(opt){
        case fs::OpeningType::Write:
            return "w";
        case fs::OpeningType::Read:
            return "r";
        case fs::OpeningType::Append:
            return "a";
        case fs::OpeningType::WriteRead:
            return "w+";
        case fs::OpeningType::AppendRead:
            return "a+";
        case fs::OpeningType::ReadWrite:
            return "r+";
        case fs::OpeningType::None:
            [[fallthrough]];
        default:
            return nullptr;
    }
}

/* Implement general read. */
read_integer inr_file_handle::read(void* dest, size_t size, size_t n) noexcept{
    if(valid() && fs::opening_type_read(opt)){
        if(last_op == fs::LastFileOperation::Writing) return 0;
        last_op = fs::LastFileOperation::Reading;
        switch(api){
            case APIs::POSIX:{
                    read_integer res = fd->read(dest, size * n);
                    return res == 0 ? 0 : res / size;
                }
            case APIs::WINDOWS:{
                    read_integer res = handle->read(dest, size * n);
                    return res == 0 ? 0 : res / size;
                }
            case APIs::STANDARD:
                return fread(dest, size, n, file);
            default:
                return 0;
        }
    }
    else{
        return 0;
    }
}

/* Create a new posix handle. */
inr_posix_handle* inr_posix_handle::inr_new_posix_handle(int fd, size_t buf_size){
    void* mem = ::operator new(sizeof(inr_posix_handle)+buf_size, std::align_val_t(alignof(inr_posix_handle)));

    inr_posix_handle* psx_handle = new(mem) inr_posix_handle(fd, (char*)mem + sizeof(inr_posix_handle), buf_size);

    return psx_handle;    
}
/* Delete the posix handle. */
void inr_posix_handle::inr_delete_posix_handle(inr_posix_handle* psx){
    psx->~inr_posix_handle();
    ::operator delete(psx, std::align_val_t(alignof(inr_posix_handle)));
}

/* In case if the platform is not posix. */
#ifndef INERTIA_POSIX
read_integer inr_posix_handle::read(void* data, size_t n) noexcept{
    (void)data;
    (void)n;
    return 0;
}
write_integer inr_posix_handle::write(const void* data, size_t n) noexcept{
    (void)data;
    (void)n;
    return 0;
}
int inr_posix_handle::flush() noexcept{
    return EOF;
}
void inr_posix_handle::close() noexcept{
    return;
}
inr_mem_file inr_posix_handle::inr_posix_memfile(const inr_posix_handle* psx){
    (void)psx;
    return {};
}
tell_integer inr_posix_handle::tell(inr_file_handle&) noexcept{
    return EOF;
}
seek_integer inr_posix_handle::seek(tell_integer off, fs::SeekType st) noexcept{
    (void)off;
    (void)st;
    return EOF;
}
#else
/* A quick munmap caller. */
static void close_mmaped_file(inr_mem_file& mf){
    if(mf.valid()){
        munmap(mf.get_file(), mf.size());
    }
}

/* POSIX tell. */
tell_integer inr_posix_handle::tell(inr_file_handle& f) noexcept{
    off_t off = lseek(fd, 0, SEEK_CUR);
    
    off_t finalv = 0;
    switch(f.last_operation()){
        default:
            [[fallthrough]];
        case fs::LastFileOperation::None:
            finalv = off;
            break;
        case fs::LastFileOperation::Writing:
            finalv = off + buff.current();
            break;
        case fs::LastFileOperation::Reading:
            finalv = off - (buff.current() - read_i);
            break;
    }

    return off < 0 ? EOF : finalv;
}

/* POSIX seek. */
seek_integer inr_posix_handle::seek(tell_integer off, fs::SeekType st) noexcept{
    off_t sres = lseek(fd, off, (int)st);
    return sres >= 0 ? 0 : EOF;
}

/* POSIX fmemopen. */
inr_mem_file inr_posix_handle::inr_posix_memfile(const inr_posix_handle* psx){
    off_t pos = lseek(psx->fd, 0, SEEK_CUR);
    off_t size = lseek(psx->fd, 0, SEEK_END);
    if(size < 0){
        return {inr_mem_file::ERROR_SEEK};
    }
    size_t new_size = (size == 0) ? 0x1000 : (size_t)size;
    lseek(psx->fd, pos, SEEK_SET);

    void* mmaped_file = mmap(nullptr, new_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, psx->fd, 0);
    if(mmaped_file == MAP_FAILED){
        return {inr_mem_file::ERROR_ALLOC};
    }

    return {mmaped_file, close_mmaped_file, new_size};
}

/* POSIX read. */
read_integer inr_posix_handle::read(void* data, size_t n) noexcept{
    if(buff.size() == 0){
        return ::read(fd, data, n);
    }

    char* dt = (char*)data;
    
    while(n){
        if(read_i < buff.current()){
            *dt++ = buff[read_i++];
            n--;
        }
        else{
            buff.flush();
            read_i = 0;
            size_t r = ::read(fd, buff.data(), buff.size());
            if(r < 0){
                return 0;
            }

            buff.set_current(r);
            if(r == 0){
                break;
            }
        }


    }
    return dt - (const char*)data;
}

/* POSIX write. */
write_integer inr_posix_handle::write(const void* data, size_t n) noexcept{
    if(buff.size() == 0){
        return ::write(fd, data, n);
    }
    for(size_t i = 0; i < n; i++){
        if(buff.add(*((const char*)data))){
            if(this->flush() == EOF) return i;
        }
        data = (const char*)data + 1;
    }

    return n;
}

/* POSIX flush. */
int inr_posix_handle::flush() noexcept{
    if(buff.current() == 0 || buff.size() == 0) return 0;
    ssize_t res = ::write(fd, buff.data(), buff.current());
    buff.flush();
    if(res < 0){
        return EOF;
    }
    return 0;
}

/* POSIX close. */
void inr_posix_handle::close() noexcept{
    ::close(fd);
}
#endif

inr_mem_file inr_windows_handle::inr_windows_memfile(const inr_windows_handle*){
    return {};
}

tell_integer inr_windows_handle::tell(inr_file_handle&) noexcept{
    return EOF;
}

seek_integer inr_windows_handle::seek(tell_integer off, fs::SeekType st) noexcept{
    (void)off;
    (void)st;
    return EOF;
}

inr_windows_handle* inr_windows_handle::inr_new_windows_handle(void* handle, size_t buf_size){
    void* mem = ::operator new(sizeof(inr_windows_handle)+buf_size, std::align_val_t(alignof(inr_windows_handle)));

    inr_windows_handle* wnd_handle = new(mem) inr_windows_handle(handle, (char*)mem + sizeof(inr_windows_handle), buf_size);

    return wnd_handle;
}

void inr_windows_handle::inr_delete_windows_handle(inr_windows_handle* wnd){
    wnd->~inr_windows_handle();
    ::operator delete(wnd, std::align_val_t(alignof(inr_windows_handle)));
}

write_integer inr_windows_handle::write(const void* data, size_t n) noexcept{
    (void)data;
    (void)n;
    return 0;
}

read_integer inr_windows_handle::read(void* data, size_t n) noexcept{
    (void)data;
    (void)n;
    return 0;
}

int inr_windows_handle::flush() noexcept{
    return EOF;
}

void inr_windows_handle::close() noexcept{
    return;
}

static void standard_close_fmem(inr_mem_file& mf){
    free(mf.get_file());
}

inr_mem_file inr_file_handle::fmem_open() const{
    if(!valid()) return {};

    switch(api){
        case APIs::POSIX:
            return inr_posix_handle::inr_posix_memfile(fd);
        case APIs::WINDOWS:
            return inr_windows_handle::inr_windows_memfile(handle);
        case APIs::STANDARD:{
                long pos = ftell(file);
                if(fseek(file, 0, SEEK_END) == EOF) return {inr_mem_file::ERROR_SEEK};
                long sz = ftell(file);
                if(sz < 0){
                    return {inr_mem_file::ERROR_SEEK};
                }
                

                size_t new_size = (sz == 0) ? 0x1000 : (size_t)sz;
                void* ptr = malloc(new_size);
                if(!ptr){
                    return {inr_mem_file::ERROR_ALLOC};
                }
                fseek(file, 0, SEEK_SET);

                fread(ptr, 1, new_size, file);

                fseek(file, pos, SEEK_SET);
                return {ptr, standard_close_fmem, new_size};
            }
        default:
            return {};
    }
}

}
