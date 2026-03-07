#ifndef INERTIA_SUPPORT_STRING_H
#define INERTIA_SUPPORT_STRING_H

/// @file Support/String.h
/// @brief Provides some string utilities, constexpr especially.

#include <inr/Support/Compiler.h>

#include <cstddef>
#include <cstring>

#if !__has_builtin(strlen)
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
        return strlen(str);
    }
#endif
}
} // namespace inr::str

#endif // INERTIA_SUPPORT_STRING_H
