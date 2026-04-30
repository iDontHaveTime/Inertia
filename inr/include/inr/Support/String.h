// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_STRING_H
#define INERTIA_SUPPORT_STRING_H

/// @file Support/String.h
/// @brief Provides some string utilities, constexpr especially.

#include <inr/Support/Compiler.h>

#include <cstddef>
#include <cstring>

#if !__has_builtin(strlen) || !__has_builtin(memcmp) || !__has_builtin(memchr)
#include <type_traits>
#endif

namespace inr::str {

/// @brief Iterates through chars in a string until a null terminator is found.
/// @param str C string.
/// @return String length.
///
/// If GNU extensions exist and builtin for strlen exists we use that.
/// If no GNU extensions are present,
/// we either calculate it manually for constexpr or rely on strlen().
/// This is mainly used so that the `sview` class can be constexpr-able.
constexpr size_t length(const char* str) noexcept {
#if __has_builtin(strlen)
    return __builtin_strlen(str);
#else
    if(std::is_constant_evaluated()) {
        size_t len = 0;
        while(str[len] != '\0') len++;
        return len;
    }
    else {
        return std::strlen(str);
    }
#endif
}

constexpr int compare(const void* s1, const void* s2, size_t n) noexcept {
#if __has_builtin(memcmp)
    return __builtin_memcmp(s1, s2, n);
#else
    if(std::is_constant_evaluated()) {
        const unsigned char* a = (const unsigned char*)s1;
        const unsigned char* b = (const unsigned char*)s2;
        for(size_t i = 0; i < n; i++) {
            if(a[i] != b[i]) {
                return (a[i] < b[i]) ? -1 : 1;
            }
        }
        return 0;
    }
    else {
        return std::memcmp(s1, s2, n);
    }
#endif
}

constexpr const void* findc(const void* s, int c, size_t n) noexcept {
#if __has_builtin(memchr)
    return __builtin_memchr(s, c, n);
#else
    if(std::is_constant_evaluated()) {
        const char* str = (const char*)s;
        for(size_t i = 0; i < n; i++) {
            if(str[i] == (unsigned char)c) return &str[i];
        }
        return nullptr;
    }
    else {
        return std::memchr(s, c, n);
    }
#endif
}

} // namespace inr::str

#endif // INERTIA_SUPPORT_STRING_H
