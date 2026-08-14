// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_ADT_HMAPINFO_H
#define INERTIA_ADT_HMAPINFO_H

/// @file ADT/HMapInfo.h
/// @brief Provides an info on how to hash and compare an element in hash ADTs.

#include <cstddef>
#include <string_view>
#include <type_traits>

namespace inr {

/// @brief Provides info on how an element should be hashed and compared.
template<typename T>
struct HMapInfo;

template<typename T>
requires std::is_integral_v<T>
struct HMapInfo<T> {
    // splitmix64
    static std::size_t hash(const T& v) {
        std::size_t x = std::size_t(v);
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    static bool equal(const T& lhs, const T& rhs) {
        return lhs == rhs;
    }
};

template<typename T>
struct HMapInfo<T*> {
    static std::size_t hash(const T* v) {
        return std::size_t(v);
    }

    static bool equal(const T* lhs, const T* rhs) {
        return lhs == rhs;
    }
};

template<>
struct HMapInfo<std::string_view> {
    static std::size_t hash(std::string_view v) {
        std::size_t h = 5381;
        for(char c : v) {
            h = ((h << 5) + h) + c;
        }
        return h;
    }

    static bool equal(std::string_view lhs, std::string_view rhs) {
        return lhs == rhs;
    }
};

} // namespace inr

#endif // INERTIA_ADT_HMAPINFO_H
