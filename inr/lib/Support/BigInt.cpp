// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/ADT/BigInt.h>

#include <algorithm>
#include <cstdint>

namespace inr {

bool bigint::bigintAdd(uint64_t* dest, const uint64_t* src, bool c,
                       size_t limbs) noexcept {
    for(size_t i = 0; i < limbs; i++) {
        uint64_t l = dest[i];
        dest[i] += src[i] + c;
        if(c) {
            c = (dest[i] <= l);
        }
        else {
            c = (dest[i] < l);
        }
    }

    return c;
}

bool bigint::bigintAddLimb(uint64_t* dest, uint64_t src,
                           size_t limbs) noexcept {
    bool carry = src != 0;
    for(size_t i = 0; i < limbs; i++) {
        uint64_t old = dest[i];
        dest[i] += src;
        carry = dest[i] < old;
        src = 1;
    }

    return carry;
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

void bigint::bigintShiftRight(uint64_t* dest, size_t limbs,
                              unsigned shiftN) noexcept {
    if(!shiftN) return;

    size_t words = std::min(limbs, size_t(shiftN >> 6));
    unsigned bits = shiftN & 63;

    size_t wordsMove = limbs - words;

    // Bits more likely
    if(bits) {
        for(size_t i = 0; i < wordsMove; i++) {
            uint64_t* src = dest + i + words;
            dest[i] = *src >> bits;

            if(i + 1 < wordsMove) {
                dest[i] |= *(src + 1) << (64 - bits);
            }
        }
    }
    else {
        std::memmove(dest, dest + words, wordsMove * sizeof(uint64_t));
    }

    std::fill(dest + wordsMove, dest + wordsMove + words, 0);
}

void bigint::print(raw_stream& os, unsigned radix, bool isSigned,
                   bool addPrefix, bool upperCase) const {
    if(radix != 2 && radix != 8 && radix != 10 && radix != 16) {
        os << "(bigint: radix is not 2, 8, 10, or 16)";
        return;
    }

    const char* prefix = "";
    if(addPrefix) {
        switch(radix) {
            case 2:
                prefix = "0b";
                break;
            case 8:
                prefix = "0";
                break;
            case 10:
                break;
            case 16:
                prefix = "0x";
                break;
            default:
                __builtin_unreachable();
        }
    }

    if(isZero()) {
        os << prefix << '0';
        return;
    }

    if(onStack()) {
        uint64_t val = stack_;
        if(isSigned && getSign()) {
            val |= ~((uint64_t(1) << bits_) - 1);
            os << (int64_t)val;
        }
        else {
            os << prefix << val;
        }
        return;
    }

    static const char* lowercase = "0123456789abcdefghijklmnopqrstuvwxyz";
    static const char* uppercase = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    const char* digits = upperCase ? uppercase : lowercase;

    bigint tmp(*this);

    if(getSign() && isSigned) {
        tmp.negate();
        os << '-';
    }

    os << prefix;

    std::string str;

    if(radix != 10) {
        unsigned shiftN = (radix == 16 ? 4 : (radix == 8 ? 3 : 1));
        unsigned maskN = radix - 1;

        while(!tmp.isZero()) {
            unsigned digit = unsigned(tmp.data()[0] & maskN);
            str.push_back(digits[digit]);
            tmp.shiftRight(shiftN);
        }
    }
    else {
        str.push_back(0);
        const uint64_t* limbs = heap_;

        for(long i = bits_ - 1; i >= 0; i--) {
            size_t limbIdx = (size_t)i >> 6;
            size_t bitIdx = (size_t)i & 63;
            int carry = (limbs[limbIdx] & (uint64_t(1) << bitIdx)) ? 1 : 0;

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
    }
    std::reverse(str.begin(), str.end());

    os << str;
}

} // namespace inr