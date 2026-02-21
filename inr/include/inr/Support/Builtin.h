#ifndef INERTIA_SUPPORT_BUILTIN_H
#define INERTIA_SUPPORT_BUILTIN_H

#include <cstring>

#ifdef __GNUC__
#define inr_restrict __restrict
#else
#define inr_restrict
#endif

namespace inr{

    inline const void* memcpy(void* inr_restrict dest, const void* inr_restrict src, size_t n) noexcept{
        #if defined(__GNUC__)
        return __builtin_memcpy(dest, src, n);
        #else
        return ::memcpy(dest, src, n);
        #endif
    }

    inline size_t strlen(const char* str) noexcept{
        #if defined(__GNUC__)
        return __builtin_strlen(str);
        #else
        return ::strlen(str);
        #endif
    }

}

#endif // INERTIA_SUPPORT_BUILTIN_H
