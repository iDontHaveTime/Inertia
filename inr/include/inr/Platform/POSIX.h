#ifndef INERTIA_PLATFORM_POSIX_H
#define INERTIA_PLATFORM_POSIX_H

#include <inr/Platform/Platform.h>

#ifdef INERTIA_POSIX
extern "C"{

#include <sys/types.h>

extern ssize_t write(int, const void*, size_t);
extern ssize_t read(int, void*, size_t);

}
#endif

namespace inr::posix{
    #ifdef INERTIA_POSIX
    using ::write;
    using ::read;
    #endif
}

#endif // INERTIA_PLATFORM_POSIX_H
