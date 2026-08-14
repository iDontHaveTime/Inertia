// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Support/Endian.h>

#include <bit>
#include <type_traits>

#define BYTE_SWAP16 __builtin_bswap16
#define BYTE_SWAP32 __builtin_bswap32
#define BYTE_SWAP64 __builtin_bswap64

namespace inr {

template<>
uint8_t hosttole(uint8_t n) {
    return n;
}

template<>
uint8_t hosttobe(uint8_t n) {
    return n;
}

template<>
uint16_t hosttole(uint16_t n) {
    if constexpr(std::endian::native == std::endian::little) {
        return n;
    }
    else {
        return BYTE_SWAP16(n);
    }
}

template<>
uint16_t hosttobe(uint16_t n) {
    if constexpr(std::endian::native == std::endian::little) {
        return BYTE_SWAP16(n);
    }
    else {
        return n;
    }
}

template<>
uint32_t hosttole(uint32_t n) {
    if constexpr(std::endian::native == std::endian::little) {
        return n;
    }
    else {
        return BYTE_SWAP32(n);
    }
}

template<>
uint32_t hosttobe(uint32_t n) {
    if constexpr(std::endian::native == std::endian::little) {
        return BYTE_SWAP32(n);
    }
    else {
        return n;
    }
}

template<>
uint64_t hosttole(uint64_t n) {
    if constexpr(std::endian::native == std::endian::little) {
        return n;
    }
    else {
        return BYTE_SWAP64(n);
    }
}

template<>
uint64_t hosttobe(uint64_t n) {
    if constexpr(std::endian::native == std::endian::little) {
        return BYTE_SWAP64(n);
    }
    else {
        return n;
    }
}

template<>
int8_t hosttole(int8_t n) {
    return n;
}

template<>
int8_t hosttobe(int8_t n) {
    return n;
}

template<>
int16_t hosttole(int16_t n) {
    return hosttole<std::make_unsigned_t<decltype(n)>>(n);
}

template<>
int16_t hosttobe(int16_t n) {
    return hosttobe<std::make_unsigned_t<decltype(n)>>(n);
}

template<>
int32_t hosttole(int32_t n) {
    return hosttole<std::make_unsigned_t<decltype(n)>>(n);
}

template<>
int32_t hosttobe(int32_t n) {
    return hosttobe<std::make_unsigned_t<decltype(n)>>(n);
}

template<>
int64_t hosttole(int64_t n) {
    return hosttole<std::make_unsigned_t<decltype(n)>>(n);
}

template<>
int64_t hosttobe(int64_t n) {
    return hosttobe<std::make_unsigned_t<decltype(n)>>(n);
}

static inline void flip_data(char* data, std::size_t size) {
    switch(size) {
        case 1:
            return;
        case 2:
            *data = BYTE_SWAP16(*(uint16_t*)data);
            return;
        case 4:
            *data = BYTE_SWAP32(*(uint32_t*)data);
            return;
        case 8:
            *data = BYTE_SWAP64(*(uint64_t*)data);
            return;
        default:
            break;
    }

    std::size_t goal = size / 2;

    for(std::size_t i = 0; i < goal; i++) {
        std::swap(data[i], data[size - 1 - i]);
    }
}

void hosttole(char* data, std::size_t size) {
    if constexpr(std::endian::native == std::endian::little) return;
    return flip_data(data, size);
}

void hosttobe(char* data, std::size_t size) {
    if constexpr(std::endian::native == std::endian::big) return;
    return flip_data(data, size);
}

void hosttoendian(char* data, std::size_t size, std::endian endian) {
    if(std::endian::native != endian) {
        if constexpr(std::endian::native == std::endian::big) {
            return hosttole(data, size);
        }
        else return hosttobe(data, size);
    }
}

} // namespace inr
