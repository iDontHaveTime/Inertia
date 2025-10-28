#ifndef INERTIA_APIS_HPP
#define INERTIA_APIS_HPP

#define INERTIA_WINDOWS_UNSUPPORTED

#include <cstdint>

/**
 * @file inr/Defines/APIs.hpp
 * @brief Inertia's defines for APIs.
 *
 * This header contains Inertia's definitions for APIs.
 * This defines APIs so that Inertia's classes can use them wisely.
 *
 **/

namespace inr{
    /**
     * @brief APIs used by Inertia's internal structures.
     */
    enum class APIs : uint8_t{
        NONE, /**< Only used in errors. */
        POSIX, /**< POSIX API. */
        WINDOWS, /**< Windows API. */
        STANDARD /**< Standard Library. */
    };
}

#if !defined(INERTIA_STANDARD) && !defined(INERTIA_POSIX) && !defined(INERTIA_WINDOWS)

#if __has_include(<unistd.h>)
#define INERTIA_POSIX
constexpr inr::APIs INERTIA_PLATFORM_API = inr::APIs::POSIX;
#elif defined(_WIN32) && !defined(INERTIA_WINDOWS_UNSUPPORTED)
#define INERTIA_WINDOWS
constexpr inr::APIs INERTIA_PLATFORM_API = inr::APIs::WINDOWS;
#else
#define INERTIA_STANDARD
constexpr inr::APIs INERTIA_PLATFORM_API = inr::APIs::STANDARD;
#endif

#endif

#undef INERTIA_WINDOWS_UNSUPPORTED

#endif // INERTIA_APIS_HPP
