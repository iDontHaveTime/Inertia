#include "inr/Support/inrfile.hpp"
#include "inr/Defines/inrapis.hpp"
#include "inr/Defines/inrfiledef.hpp"
#include <cstdio>
#include <cstdlib>
#include <new>

#ifdef INERTIA_POSIX
#include <unistd.h>
#include <sys/mman.h>
#endif

namespace inr{

/* API specific write. */
long inr_file_handle::write(const void* data, size_t size, size_t n) noexcept{
    if(valid() && (uint8_t)opt & 0x1){
        if(last_op == fs::LastFileOperation::Reading) return EOF;
        last_op = fs::LastFileOperation::Writing;
        switch(api){
            case APIs::POSIX:{
                    long res = fd->write(data, size * n);
                    return res == EOF ? EOF : res / size;
                }
            case APIs::WINDOWS:{
                    long res = handle->write(data, size * n);
                    return res == EOF ? EOF : res / size;
                }
            case APIs::STANDARD:
                return fwrite(data, size, n, file);
            default:
                return EOF;
        }
    }
    else{
        return EOF;
    }
}

int inr_file_handle::flush() noexcept{
    if(valid()){
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

int inr_file_handle::read(void* dest, size_t size, size_t n) noexcept{
    if(valid() && (uint8_t)opt & 0x40){
        if(last_op == fs::LastFileOperation::Writing) return EOF;
        last_op = fs::LastFileOperation::Reading;
        switch(api){
            case APIs::POSIX:
                return EOF;
            case APIs::WINDOWS:
                return EOF;
            case APIs::STANDARD:
                return fread(dest, size, n, file);
            default:
                return EOF;
        }
    }
    else{
        return EOF;
    }
}


inr_posix_handle* inr_posix_handle::inr_new_posix_handle(int fd, size_t buf_size){
    void* mem = ::operator new(sizeof(inr_posix_handle)+buf_size, std::align_val_t(alignof(inr_posix_handle)));

    inr_posix_handle* psx_handle = new(mem) inr_posix_handle(fd, (char*)mem + sizeof(inr_posix_handle), buf_size);

    return psx_handle;    
}
void inr_posix_handle::inr_delete_posix_handle(inr_posix_handle* psx){
    psx->~inr_posix_handle();
    ::operator delete(psx, std::align_val_t(alignof(inr_posix_handle)));
}

#ifndef INERTIA_POSIX
long inr_posix_handle::write(const void* data, size_t n){
    (void)data;
    (void)n;
    return EOF;
}
int inr_posix_handle::flush(){
    return EOF;
}
void inr_posix_handle::close(){
    return;
}
inr_mem_file inr_posix_handle::inr_posix_memfile(const inr_posix_handle* psx){
    return {};
}
#else
static void close_mmaped_file(inr_mem_file& mf){
    if(mf.valid()){
        munmap(mf.get_file(), mf.size());
    }
}
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
long inr_posix_handle::write(const void* data, size_t n){
    if(buff.size() == 0){
        return ::write(fd, data, n);
    }
    for(size_t i = 0; i < n; i++){
        if(buff.add(*((const char*)data))){
            if(this->flush() == EOF) return EOF;
        }
        data = (const char*)data + 1;
    }

    return n;
}
int inr_posix_handle::flush(){
    if(buff.current() == 0 || buff.size() == 0) return 0;
    ssize_t res = ::write(fd, buff.data(), buff.current());
    buff.flush();
    if(res < 0){
        return EOF;
    }
    return 0;
}
void inr_posix_handle::close(){
    ::close(fd);
}
#endif

inr_mem_file inr_windows_handle::inr_windows_memfile(const inr_windows_handle*){
    return {};
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

long inr_windows_handle::write(const void* data, size_t n){
    (void)data;
    (void)n;
    return EOF;
}

int inr_windows_handle::flush(){
    return EOF;
}

void inr_windows_handle::close(){
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
