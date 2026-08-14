// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_ENDIAN
#define INERTIA_SUPPORT_ENDIAN

/// @file Support/Endian.h
/// @brief Provides endian related helpers.

#include <bit>
#include <cstdint>

namespace inr {

/// @brief Converts from host endian to little endian.
template<typename T>
T hosttole(T n) = delete;

/// @brief Converts from host endian to big endian.
template<typename T>
T hosttobe(T n) = delete;

template<>
uint8_t hosttole(uint8_t n);

template<>
uint8_t hosttobe(uint8_t n);

template<>
uint16_t hosttole(uint16_t n);

template<>
uint16_t hosttobe(uint16_t n);

template<>
uint32_t hosttole(uint32_t n);

template<>
uint32_t hosttobe(uint32_t n);

template<>
uint64_t hosttole(uint64_t n);

template<>
uint64_t hosttobe(uint64_t n);

template<>
int8_t hosttole(int8_t n);

template<>
int8_t hosttobe(int8_t n);

template<>
int16_t hosttole(int16_t n);

template<>
int16_t hosttobe(int16_t n);

template<>
int32_t hosttole(int32_t n);

template<>
int32_t hosttobe(int32_t n);

template<>
int64_t hosttole(int64_t n);

template<>
int64_t hosttobe(int64_t n);

template<typename T>
T hosttoendian(T val, std::endian endian) {
    if(std::endian::native == endian) {
        return val;
    }
    else {
        if constexpr(std::endian::native == std::endian::big) {
            return hosttole(val);
        }
        else return hosttobe(val);
    }
}

/// @brief Flips the data to little endian.
void hosttole(char* data, std::size_t size);
/// @brief Flips the data to big endian.
void hosttobe(char* data, std::size_t size);

void hosttoendian(char* data, std::size_t size, std::endian endian);

} // namespace inr

#endif // INERTIA_SUPPORT_ENDIAN
