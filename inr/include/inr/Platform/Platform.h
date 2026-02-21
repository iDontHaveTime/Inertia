#ifndef INERTIA_PLATFORM_PLATFORM_H
#define INERTIA_PLATFORM_PLATFORM_H

#ifdef _WIN32
#define INERTIA_WINDOWS
#endif

#ifdef __unix__
#define INERTIA_UNIX

#if __has_include(<unistd.h>)
#define INERTIA_POSIX
#endif

#endif

#endif // INERTIA_PLATFORM_PLATFORM_H
