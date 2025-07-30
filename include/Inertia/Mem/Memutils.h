#ifndef INERTIA_MEMUTILS_H
#define INERTIA_MEMUTILS_H

#ifdef __cplusplus
#include <cstdlib>
extern "C" {
#else
#include <stdlib.h>
#endif  

void* aligned_malloc(size_t size, size_t alignment);
// Only needed on windows
void aligned_free(void* ptr);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace MemUtils{
    inline void* aligned_malloc(size_t size, size_t alignment) noexcept{
        return ::aligned_malloc(size, alignment);
    }
    inline void aligned_free(void* ptr) noexcept{
        ::aligned_free(ptr);
    }
}
#endif

#endif // INERTIA_MEMUTILS_H
