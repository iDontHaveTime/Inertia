#include "Inertia/Mem/Memutils.h"

extern "C" void* aligned_malloc(size_t size, size_t alignment){
    if(!size || !alignment) return nullptr;
    #ifdef _WIN32
    return _aligned_malloc(size, alignment);
    #else
    void* ptr = nullptr;
    if(posix_memalign(&ptr, alignment, size) != 0) return nullptr;
    return ptr;
    #endif
}

extern "C" void aligned_free(void* ptr){
    if(!ptr) return; 
    #ifdef _WIN32
    _aligned_free(ptr);
    #else
    free(ptr);
    #endif
}