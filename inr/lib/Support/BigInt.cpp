// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/ADT/BigInt.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>

#define USE_X86LIBS __x86_64__

#if USE_X86LIBS
#define DO_NOT_USE_BUILTIN
#endif

#if USE_X86LIBS || defined(__aarch64__)
#define USE_SECOND_BASE10IMPL
#endif

#ifdef USE_X86LIBS
#include <immintrin.h>

namespace inr {
bool bigint::bigintAdd(Limb* dest, const Limb* src, bool c,
                       size_t limbs) noexcept {
    unsigned char carry = c;

    for(size_t i = 0; i < limbs; i++) {
        carry = _addcarry_u64(carry, dest[i], src[i], &dest[i]);
    }

    return carry;
}

bool bigint::bigintAddLimb(Limb* dest, Limb src, size_t limbs) noexcept {
    unsigned char carry = _addcarry_u64(0, dest[0], src, &dest[0]);
    size_t i = 1;
    while(carry && i < limbs) {
        carry = _addcarry_u64(carry, dest[i], 0, &dest[i]);
        i++;
    }
    return carry;
}
} // namespace inr
#endif

#ifndef DO_NOT_USE_BUILTIN
namespace inr {
bool bigint::bigintAdd(Limb* dest, const Limb* src, bool c,
                       size_t limbs) noexcept {
    unsigned long long carry = c;

    for(size_t i = 0; i < limbs; i++) {
        carry = __builtin_addcll(dest[i], src[i], carry, &dest[i]);
    }

    return carry;
}

bool bigint::bigintAddLimb(Limb* dest, Limb src, size_t limbs) noexcept {
    unsigned long long carry = __builtin_addcll(dest[0], src, 0, &dest[0]);

    size_t i = 1;
    while(carry && i < limbs) {
        carry = __builtin_addcll(dest[i], 0, carry, &dest[i]);
        i++;
    }

    return carry;
}
} // namespace inr
#endif

namespace inr {
#ifndef USE_SECOND_BASE10IMPL
void bigint::base10Impl(bigint& tmp, raw_stream& os) {
    std::string str;
    str.push_back(0);
    const Limb* limbs = tmp.heap_;

    for(long i = tmp.bits_ - 1; i >= 0; i--) {
        size_t limbIdx = (size_t)i >> LIMB_DIV;
        size_t bitIdx = (size_t)i & ((1 << LIMB_DIV) - 1);
        int carry = (limbs[limbIdx] & (Limb(1) << bitIdx)) ? 1 : 0;

        for(char& digit : str) {
            int val = (digit << 1) | carry;
            digit = val % 10;
            carry = val / 10;
        }

        if(carry) {
            str.push_back(carry);
        }
    }

    for(char& digit : str) {
        digit += '0';
    }

    if(os.getBufferSize() > 0x1000) {
        for(auto it = str.rbegin(); it != str.rend(); ++it) {
            os << *it;
        }
    }
    else {
        std::reverse(str.begin(), str.end());
        os << str;
    }
}
#else
#include <alloca.h>

void bigint::base10Impl(bigint& tmp, raw_stream& os) {
    uint32_t* chunks = nullptr;
    size_t maxChunks = tmp.bits_ >> 3;
    size_t chunkCount = 0;

    if(maxChunks <= 512) {
        chunks = (uint32_t*)alloca(maxChunks * sizeof(uint32_t));
    }
    else chunks = new uint32_t[maxChunks];

    while(!tmp.isZero()) {
        uint64_t remainder = 0;

        for(size_t i = tmp.size(); i-- > 0;) {
            unsigned __int128 val =
                ((unsigned __int128)remainder << 64) | tmp.heap_[i];
            tmp.heap_[i] = val / 1000000000ULL;
            remainder = val % 1000000000ULL;
        }

        chunks[chunkCount++] = (uint32_t)remainder;
    }

    if(maxChunks > 512) {
        delete[] chunks;
    }

    os << chunks[chunkCount - 1];

    for(long i = chunkCount - 1; i-- > 0;) {
        uint32_t n = chunks[i];
        char buf[9];

        for(int j = 8; j >= 0; j--) {
            buf[j] = '0' + (n % 10);
            n /= 10;
        }

        os.write(buf, 9);
    }
}
#endif
} // namespace inr

namespace inr {

void bigint::bigintShiftRight(Limb* dest, size_t limbs,
                              unsigned shiftN) noexcept {
    if(!shiftN) return;

    size_t words = std::min(limbs, size_t(shiftN >> LIMB_DIV));
    unsigned bits = shiftN & ((1 << LIMB_DIV) - 1);
    size_t wordsMove = limbs - words;

    if(bits) {
        for(size_t i = 0; i + 1 < wordsMove; i++) {
            dest[i] = (dest[i + words] >> bits) |
                      (dest[i + words + 1] << (LIMB_BITS - bits));
        }
        dest[wordsMove - 1] = dest[wordsMove - 1 + words] >> bits;
    }
    else {
        std::memmove(dest, dest + words, wordsMove * sizeof(Limb));
    }

    std::fill(dest + wordsMove, dest + wordsMove + words, 0);
}

bigint& bigint::operator+=(const bigint& other) {
    checkBitWidths(other);
    if(onStack()) {
        stack_ += other.stack_;
    }
    else {
        bigintAdd(heap_, other.heap_, false, size());
    }
    return zeroOutTopBits();
}

void bigint::print(raw_stream& os, unsigned radix, bool isSigned,
                   bool addPrefix, bool upperCase) const {
    if(radix != 2 && radix != 8 && radix != 10 && radix != 16) {
        os << "(bigint: radix is not 2, 8, 10, or 16)";
        return;
    }

    if(addPrefix) {
        switch(radix) {
            case 2:
                os << "0b";
                break;
            case 8:
                os << "0";
                break;
            case 10:
                break;
            case 16:
                os << "0x";
                break;
            default:
                __builtin_unreachable();
        }
    }

    if(isZero()) {
        os << '0';
        return;
    }

    if(onStack()) {
        uint64_t val = stack_;
        if(isSigned && getSign()) {
            val |= ~((Limb(1) << bits_) - 1);
            os << (SLimb)val;
        }
        else {
            os << val;
        }
        return;
    }

    const char* digits = upperCase ? "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   : "0123456789abcdefghijklmnopqrstuvwxyz";

    bigint tmp(*this);

    if(getSign() && isSigned) {
        tmp.negate();
        os << '-';
    }

    if(radix != 10) {
        std::string str;
        unsigned shiftN = (radix == 16 ? 4 : (radix == 8 ? 3 : 1));
        unsigned maskN = radix - 1;

        while(!tmp.isZero()) {
            unsigned digit = unsigned(tmp.data()[0] & maskN);
            str.push_back(digits[digit]);
            tmp.shiftRight(shiftN);
        }
        std::reverse(str.begin(), str.end());

        os << str;
    }
    else {
        base10Impl(tmp, os);
    }
}

} // namespace inr