// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Math/BigInt.h>
#include <inr/Support/Assert.h>
#include <inr/Support/Stream.h>
#include <inr/Support/Unreachable.h>

#include <algorithm>
#include <bit>
#include <charconv>
#include <cstddef>
#include <cstring>
#include <utility>

namespace inr {

unsigned bigint::allocateNewStorage(unsigned bits) {
    inr_assert(bits != 0,
               "bigint allocateNewStorage(): tried to allocate 0 bits");
    freeStorage();

    bits_ = bits;
    if(isOnHeap()) {
        unsigned limbc = (bits + (LIMB_BITS - 1)) / LIMB_BITS;
        heapStorage_ = new Limb[limbc]();
        return limbc;
    }

    inlineStorage_ = 0;
    return 0;
}

void bigint::resizeStorage(unsigned bits, bool signExt) {
    if(bits_ == bits) return;

    bigint newBigint(bits);

    if(bits > bits_) {
        std::memcpy(newBigint.getData(), getData(),
                    getLimbCount() * sizeof(Limb));

        if(signExt && getSignBit()) {
            for(unsigned limb = getLimbCount(); limb < newBigint.getLimbCount();
                limb++) {
                // For stack limb is equal to 1, thus if both were stack this
                // loop wouldn't run.
                newBigint.heapStorage_[limb] = ~Limb(0);
            }
            if(unsigned tbits = bits_ % LIMB_BITS) {
                Limb mask = ~Limb(0) << tbits;
                newBigint.getData()[getLimbCount() - 1] |= mask;
            }
        }
    }
    else {
        std::memcpy(newBigint.getData(), getData(),
                    newBigint.getLimbCount() * sizeof(Limb));
        newBigint.clearTopBits();
    }

    *this = std::move(newBigint);
}

bigint::Limb* bigint::getLastHeapLimb() {
    return &heapStorage_[(getAllocatedBits() / LIMB_BITS) - 1];
}

const bigint::Limb* bigint::getLastHeapLimb() const {
    return &heapStorage_[(getAllocatedBits() / LIMB_BITS) - 1];
}

void bigint::clearTopBits() {
    unsigned allocB = getAllocatedBits();
    if(unsigned btoc = allocB - bits_) {
        Limb mask = (~Limb(0)) >> btoc;
        if(isOnHeap()) {
            *getLastHeapLimb() &= mask;
        }
        else inlineStorage_ &= mask;
    }
}

void bigint::freeStorage() {
    if(isOnHeap()) delete[] heapStorage_;
}

bigint::bigint() {
    allocateNewStorage(1);
}

bigint::bigint(unsigned bits) {
    inr_assert(bits != 0,
               "bigint bigint(unsigned): tried making a bigint with zero bits");
    allocateNewStorage(bits);
}

bigint::bigint(unsigned bits, Limb val, bool signExt) : bigint(bits) {
    bitOr(val, signExt);
}

bigint::bigint(const bigint& other) {
    unsigned limbs = allocateNewStorage(other.bits_);
    if(limbs) {
        std::memcpy(heapStorage_, other.heapStorage_, sizeof(Limb) * limbs);
    }
    else {
        inlineStorage_ = other.inlineStorage_;
    }
}

bigint& bigint::operator=(const bigint& other) {
    if(this != &other) {
        freeStorage();

        unsigned limbs = allocateNewStorage(other.bits_);

        if(limbs) {
            std::memcpy(heapStorage_, other.heapStorage_, sizeof(Limb) * limbs);
        }
        else {
            inlineStorage_ = other.inlineStorage_;
        }
    }
    return *this;
}

bigint::bigint(bigint&& other) noexcept {
    bits_ = other.bits_;
    if(isOnHeap()) {
        heapStorage_ = other.heapStorage_;
        other.heapStorage_ = nullptr;
    }
    else inlineStorage_ = other.inlineStorage_;
    other.bits_ = 0;
}

bigint& bigint::operator=(bigint&& other) noexcept {
    if(this != &other) {
        freeStorage();
        bits_ = other.bits_;

        if(isOnHeap()) {
            heapStorage_ = other.heapStorage_;
            other.heapStorage_ = nullptr;
        }
        else {
            inlineStorage_ = other.inlineStorage_;
        }

        other.bits_ = 0;
    }
    return *this;
}

bool bigint::getBit(unsigned b) const {
    inr_assert(b < bits_, "bigint getBit(): out of range");
    if(isOnHeap()) {
        unsigned limbc = b / LIMB_BITS;
        unsigned bitc = b % LIMB_BITS;

        return heapStorage_[limbc] & Limb(1) << bitc;
    }
    else return inlineStorage_ & Limb(1) << b;
}

void bigint::setBit(unsigned b) {
    inr_assert(b < bits_, "bigint setBit(): out of range");
    if(isOnHeap()) {
        unsigned limbc = b / LIMB_BITS;
        unsigned bitc = b % LIMB_BITS;

        heapStorage_[limbc] |= Limb(1) << bitc;
    }
    else inlineStorage_ |= Limb(1) << b;
}

void bigint::setBits() {
    std::memset(getData(), -1, getLimbCount() * sizeof(Limb));
    clearTopBits();
}

void bigint::clearBit(unsigned b) {
    inr_assert(b < bits_, "bigint clearBit(): out of range");
    if(isOnHeap()) {
        unsigned limbc = b / LIMB_BITS;
        unsigned bitc = b % LIMB_BITS;

        heapStorage_[limbc] &= ~(Limb(1) << bitc);
    }
    else inlineStorage_ &= ~(Limb(1) << b);
}

void bigint::clearBits() {
    std::memset(getData(), 0, getLimbCount() * sizeof(Limb));
}

bigint bigint::truncate(unsigned b) const {
    inr_assert(b <= bits_,
               "bigint truncate(): extending instead of truncating");
    bigint cpy(*this);
    cpy.resizeStorage(b, false);
    return cpy;
}

bigint bigint::signext(unsigned b) const {
    inr_assert(b >= bits_, "bigint signext(): truncating instead of extending");
    bigint cpy(*this);
    cpy.resizeStorage(b, true);
    return cpy;
}

bigint bigint::zeroext(unsigned b) const {
    inr_assert(b >= bits_, "bigint zeroext(): truncating instead of extending");
    bigint cpy(*this);
    cpy.resizeStorage(b, false);
    return cpy;
}

bigint bigint::trunczero(unsigned b) const {
    bigint cpy(*this);
    cpy.resizeStorage(b, false);
    return cpy;
}

bigint bigint::truncsign(unsigned b) const {
    bigint cpy(*this);
    cpy.resizeStorage(b, true);
    return cpy;
}

bool bigint::isZero() const {
    if(isOnHeap()) {
        constexpr std::size_t ZERO_BUFFER_LIMBC = 64;
        constexpr static char ZERO_BUFFER[sizeof(Limb) * ZERO_BUFFER_LIMBC] = {
            0};
        unsigned limbc = getLimbCount();

        if(limbc < ZERO_BUFFER_LIMBC) {
            return std::memcmp(heapStorage_, ZERO_BUFFER,
                               limbc * sizeof(Limb)) == 0;
        }

        const char* cmp_start = (const char*)(heapStorage_);
        const char* cmp_end = (const char*)(heapStorage_ + limbc);

        while(cmp_start != cmp_end) {
            std::size_t n =
                std::min<std::size_t>(sizeof(ZERO_BUFFER), cmp_end - cmp_start);

            if(std::memcmp(cmp_start, ZERO_BUFFER, n) != 0) return false;
            cmp_start += n;
        }

        return true;
    }
    else return inlineStorage_ == 0;
}

void bigint::flipBits() {
    if(isOnHeap()) {
        for(unsigned i = 0; i < getLimbCount(); i++) {
            heapStorage_[i] = ~heapStorage_[i];
        }
    }
    else inlineStorage_ = ~inlineStorage_;
    clearTopBits();
}

stream& operator<<(stream& os, const bigint& bi) {
    bi.print(os, 10, true, false, false);
    return os;
}

void bigint::bib10impl(const bigint& tmp, stream& os) {
    std::string str;
    str.reserve(tmp.bits_);
    str.push_back(0);
    const Limb* limbs = tmp.heapStorage_;

    for(long i = tmp.bits_ - 1; i >= 0; i--) {
        unsigned limbIdx = i / LIMB_BITS;
        unsigned bitIdx = i % LIMB_BITS;
        int carry = (limbs[limbIdx] & (Limb(1) << bitIdx)) ? 1 : 0;

        for(char& digit : str) {
            int val = (digit << 1) | carry;
            digit = char(val % 10);
            carry = val / 10;
        }

        if(carry) {
            str.push_back(char(carry));
        }
    }

    for(char& digit : str) {
        digit += '0';
    }

    std::reverse(str.begin(), str.end());
    os << str;
}

void bigint::print(stream& os, unsigned radix, bool isSigned, bool addPrefix,
                   bool upperCase) const {
    inr_assert((radix == 2) || (radix == 8) || (radix == 10) || (radix == 16),
               "bigint print(): radix isn't 2, 8, 10, or 16");

    if(addPrefix) {
        switch(radix) {
            case 2:
                os.write("0b", 2);
                break;
            case 8:
                os << '0';
                break;
            case 16:
                os.write("0x", 2);
                break;
            default:
                break;
        }
    }

    if(isZero()) {
        os << '0';
        return;
    }

    if(isOnHeap()) {
        if(radix != 10) {
            bigint tmp(*this);
            const char* digits =
                upperCase ? "0123456789ABCDEF" : "0123456789abcdef";

            std::string str;
            str.reserve(bits_);

            unsigned shiftN = (radix == 16 ? 4 : (radix == 8 ? 3 : 1));
            unsigned maskN = radix - 1;

            while(!tmp.isZero()) {
                unsigned digit = unsigned(tmp.heapStorage_[0] & maskN);
                str.push_back(digits[digit]);
                tmp.shr(shiftN);
            }
            std::reverse(str.begin(), str.end());

            os << str;
        }
        else {
            if(isSigned && getSignBit()) {
                bigint tmp(*this);
                os << '-';
                tmp.negate();
                bib10impl(tmp, os);
            }
            else bib10impl(*this, os);
        }
    }
    else {
        if(radix == 10) {
            if(isSigned && getSignBit()) {
                uint64_t v = inlineStorage_ | ~((Limb(1) << (bits_ - 1)) - 1);
                os << int64_t(v);
                return;
            }
            os << inlineStorage_;
            return;
        }

        if(radix == 2) {
            char buff[LIMB_BITS + 1];
            auto res =
                std::to_chars(buff, buff + sizeof(buff), inlineStorage_, 2);
            if(res.ec == std::errc()) {
                os.write(buff, res.ptr - buff);
            }
            return;
        }

        if(radix == 16) {
            char buff[(LIMB_BITS / 4) + 1];
            auto res =
                std::to_chars(buff, buff + sizeof(buff), inlineStorage_, 16);
            if(res.ec == std::errc()) {
                os.write(buff, res.ptr - buff);
            }
            return;
        }

        if(radix == 8) {
            char buff[((LIMB_BITS + 2) / 3) + 1];
            auto res =
                std::to_chars(buff, buff + sizeof(buff), inlineStorage_, 8);
            if(res.ec == std::errc()) {
                os.write(buff, res.ptr - buff);
            }
            return;
        }
    }
}

void bigint::bishrimpl(Limb* dest, unsigned limbc, unsigned shiftN) {
    if(!shiftN) return;

    unsigned limbs = std::min(limbc, shiftN / LIMB_BITS);
    unsigned bits = shiftN % LIMB_BITS;
    unsigned limbMove = limbc - limbs;

    if(bits) {
        for(unsigned i = 0; i + 1 < limbMove; i++) {
            dest[i] = (dest[i + limbs] >> bits) |
                      (dest[i + limbs + 1] << (LIMB_BITS - bits));
        }
        dest[limbMove - 1] = dest[limbMove - 1 + limbs] >> bits;
    }
    else {
        std::memmove(dest, dest + limbs, limbMove * sizeof(Limb));
    }

    std::fill(dest + limbMove, dest + limbMove + limbs, 0);
}

void bigint::shr(unsigned n) {
    bishrimpl(getData(), getLimbCount(), n);
    clearTopBits();
}

bool bigint::bisubimpl(Limb* dest, const Limb* src, bool b, unsigned limbc) {
    Limb borrow = b;

    for(unsigned i = 0; i < limbc; i++) {
        Limb a = dest[i];
        Limb bL = src[i];

        Limb diff = a - bL;
        Limb newB = (diff > a);

        Limb finalDiff = diff - borrow;
        newB |= (finalDiff > diff);

        dest[i] = finalDiff;
        borrow = newB;
    }

    return borrow;
}

bool bigint::bisubimpllimb(Limb* dest, Limb src, unsigned limbc) {
    Limb old = dest[0];
    dest[0] = old - src;

    Limb borrow = (dest[0] > old);

    for(unsigned i = 1; borrow && i < limbc; i++) {
        Limb prev = dest[i];
        dest[i] = prev - 1;

        borrow = (prev == 0);
    }

    return borrow;
}

bool bigint::biaddimpl(Limb* dest, const Limb* src, bool c, unsigned limbc) {
    Limb carry = c;

    for(unsigned i = 0; i < limbc; i++) {
        Limb a = dest[i];
        Limb b = src[i];

        Limb sum = a + b;
        Limb new_carry = (sum < a);

        sum += carry;
        new_carry |= (sum < carry);

        dest[i] = sum;
        carry = new_carry;
    }

    return carry;
}

bool bigint::biaddimpllimb(Limb* dest, Limb src, unsigned limbc) {
    Limb old = dest[0];
    dest[0] = old + src;
    Limb carry = (dest[0] < old);

    for(unsigned i = 1; carry && i < limbc; i++) {
        Limb prev = dest[i];
        dest[i] = prev + 1;
        carry = (dest[i] == 0);
    }

    return carry;
}

bool bigint::add(Limb val) {
    bool c = biaddimpllimb(getData(), val, getLimbCount());
    clearTopBits();
    return c;
}

bool bigint::add(const bigint& other) {
    inr_assert(bits_ == other.bits_, "bigint add(): bits do not match");
    bool c = biaddimpl(getData(), other.getData(), false, getLimbCount());
    clearTopBits();
    return c;
}

bool bigint::sub(Limb val) {
    bool b = bisubimpllimb(getData(), val, getLimbCount());
    clearTopBits();
    return b;
}

bool bigint::sub(const bigint& other) {
    inr_assert(bits_ == other.bits_, "bigint sub(): bits do not match");
    bool b = bisubimpl(getData(), other.getData(), false, getLimbCount());
    clearTopBits();
    return b;
}

bigint bigint::operator~() const {
    bigint cpy(*this);
    cpy.flipBits();
    return cpy;
}

bigint& bigint::operator>>=(unsigned n) {
    shr(n);
    return *this;
}

bigint bigint::operator>>(unsigned n) const {
    bigint cpy(*this);
    cpy.shr(n);
    return cpy;
}

bigint& bigint::operator>>=(const bigint& other) {
    shr(other);
    return *this;
}

bigint bigint::operator>>(const bigint& other) const {
    bigint cpy(*this);
    cpy.shr(other);
    return cpy;
}

bigint& bigint::operator+=(Limb val) {
    add(val);
    return *this;
}

bigint& bigint::operator+=(const bigint& other) {
    add(other);
    return *this;
}

bigint bigint::operator+(Limb val) const {
    bigint cpy(*this);
    cpy.add(val);
    return cpy;
}

bigint bigint::operator+(const bigint& other) const {
    bigint cpy(*this);
    cpy.add(other);
    return cpy;
}

bigint& bigint::operator++() {
    add(1);
    return *this;
}

bigint bigint::operator++(int) {
    bigint cpy(*this);
    ++(*this);
    return cpy;
}

bigint& bigint::operator-=(Limb val) {
    sub(val);
    return *this;
}

bigint& bigint::operator-=(const bigint& other) {
    sub(other);
    return *this;
}

bigint bigint::operator-(Limb val) const {
    bigint cpy(*this);
    cpy -= val;
    return cpy;
}

bigint bigint::operator-(const bigint& other) const {
    bigint cpy(*this);
    cpy -= other;
    return cpy;
}

bigint& bigint::operator--() {
    sub(1);
    return *this;
}

bigint bigint::operator--(int) {
    bigint cpy(*this);
    --(*this);
    return cpy;
}

void bigint::negate() {
    flipBits();
    ++(*this);
}

bigint bigint::operator-() const {
    bigint cpy(*this);
    cpy.negate();
    return cpy;
}

unsigned bigint::popcount() const {
    if(isOnHeap()) {
        unsigned bits = 0;
        for(unsigned i = 0; i < getLimbCount(); i++) {
            bits += std::popcount(heapStorage_[i]);
        }
        return bits;
    }
    else return std::popcount(inlineStorage_);
}

unsigned bigint::countlz() const {
    if(isOnHeap()) {
        unsigned bits = 0;
        for(unsigned i = getLimbCount(); i-- > 0;) {
            unsigned b = std::countl_zero(heapStorage_[i]);
            bits += b;
            if(b != LIMB_BITS) break;
        }
        return bits - (getAllocatedBits() - bits_);
    }
    else return std::countl_zero(inlineStorage_) - (LIMB_BITS - bits_);
}

unsigned bigint::countlo() const {
    if(isOnHeap()) {
        unsigned bits = 0;
        unsigned shiftc = (getAllocatedBits() - bits_);
        if(bits += std::countl_one(heapStorage_[getLimbCount() - 1] << shiftc);
           bits != LIMB_BITS - shiftc)
            return bits;

        for(unsigned i = getLimbCount() - 1; i-- > 0;) {
            unsigned b = std::countl_one(heapStorage_[i]);
            bits += b;
            if(b != LIMB_BITS) break;
        }
        return bits;
    }
    else return std::countl_one(inlineStorage_ << (LIMB_BITS - bits_));
}

unsigned bigint::countrz() const {
    if(isOnHeap()) {
        unsigned bits = 0;
        for(unsigned i = 0; i < getLimbCount(); i++) {
            unsigned b = std::countr_zero(heapStorage_[i]);
            bits += b;
            if(b != LIMB_BITS) break;
        }
        return std::min(bits, bits_);
    }
    else {
        return std::min<unsigned>(std::countr_zero(inlineStorage_), bits_);
    }
}

unsigned bigint::countro() const {
    if(isOnHeap()) {
        unsigned bits = 0;
        for(unsigned i = 0; i < getLimbCount(); i++) {
            unsigned b = std::countr_one(heapStorage_[i]);
            bits += b;
            if(b != LIMB_BITS) break;
        }
        return bits;
    }
    else return std::countr_one(inlineStorage_);
}

bool bigint::operator==(const bigint& other) const {
    inr_assert(bits_ == other.bits_,
               "bigint operator==(): bit count does not match");
    if(isOnHeap()) {
        return std::memcmp(heapStorage_, other.heapStorage_,
                           getLimbCount() * sizeof(Limb)) == 0;
    }
    else return inlineStorage_ == other.inlineStorage_;
}

bool bigint::operator!=(const bigint& other) const {
    return !(*this == other);
}

bool bigint::operator==(Limb val) const {
    return cmp(val) & EQUAL;
}

bool bigint::operator!=(Limb val) const {
    return !(*this == val);
}

/// @brief Assumes lhs and rhs is zeroed out if bits < LIMB_BITS.
static inline bigint::CmpRes compare_two_limbs(bigint::Limb lhs,
                                               bigint::Limb rhs,
                                               unsigned bits) {
    if(lhs == rhs) return bigint::EQUAL;

    bigint::Limb signMask = bigint::Limb(1) << (bits - 1);

    bool lhs_sign = lhs & signMask;
    bool rhs_sign = rhs & signMask;

    if(lhs_sign) {
        if(!rhs_sign) return bigint::CmpRes(bigint::ABOVE | bigint::LESS);
    }
    else {
        if(rhs_sign) return bigint::CmpRes(bigint::BELOW | bigint::GREATER);
    }

    if(lhs > rhs) return bigint::CmpRes(bigint::ABOVE | bigint::GREATER);
    else return bigint::CmpRes(bigint::BELOW | bigint::LESS);
}

static inline bigint::CmpRes compare_two_limbarr(bigint::Limb* lhs,
                                                 bigint::Limb* rhs,
                                                 unsigned limbc,
                                                 unsigned bits) {
    bigint::Limb signMask = bigint::Limb(1) << ((bits - 1) % bigint::LIMB_BITS);
    unsigned limbIdx = (bits - 1) / bigint::LIMB_BITS;

    bool lhs_sign = lhs[limbIdx] & signMask;
    bool rhs_sign = rhs[limbIdx] & signMask;

    if(lhs_sign) {
        if(!rhs_sign) return bigint::CmpRes(bigint::ABOVE | bigint::LESS);
    }
    else {
        if(rhs_sign) return bigint::CmpRes(bigint::BELOW | bigint::GREATER);
    }

    for(unsigned i = limbc; i-- > 0;) {
        auto lhs_val = lhs[i];
        auto rhs_val = rhs[i];

        if(lhs_val > rhs_val)
            return bigint::CmpRes(bigint::ABOVE | bigint::GREATER);
        else if(lhs_val < rhs_val)
            return bigint::CmpRes(bigint::BELOW | bigint::LESS);
    }

    return bigint::EQUAL;
}

static inline bigint::CmpRes compare_two_limbs_arr_signext(bigint::Limb* lhs,
                                                           bigint::Limb rhs,
                                                           unsigned limbc,
                                                           unsigned bits,
                                                           bool signExt) {
    bool rhs_sign = rhs & (bigint::Limb(1) << (bigint::LIMB_BITS - 1));
    bigint::Limb signMask = bigint::Limb(1) << ((bits - 1) % bigint::LIMB_BITS);
    unsigned limbIdx = (bits - 1) / bigint::LIMB_BITS;

    bool lhs_sign = lhs[limbIdx] & signMask;

    if(lhs_sign) {
        if(!rhs_sign) return bigint::CmpRes(bigint::ABOVE | bigint::LESS);
    }
    else {
        if(rhs_sign) return bigint::CmpRes(bigint::BELOW | bigint::GREATER);
    }

    bigint::Limb rhs_val = (signExt && rhs_sign) ? bigint::Limb(-1) : 0;

    {
        auto lhs_val = lhs[limbc - 1];
        auto rhs_val2 = rhs_val;
        auto bOff = bits % bigint::LIMB_BITS;
        if(bOff != 0) {
            rhs_val2 &= (bigint::Limb(1) << bOff) - 1;
        }

        if(lhs_val > rhs_val2)
            return bigint::CmpRes(bigint::ABOVE | bigint::GREATER);
        else if(lhs_val < rhs_val2)
            return bigint::CmpRes(bigint::BELOW | bigint::LESS);
    }

    for(unsigned i = limbc - 1; i-- > 1;) {
        auto lhs_val = lhs[i];

        if(lhs_val > rhs_val)
            return bigint::CmpRes(bigint::ABOVE | bigint::GREATER);
        else if(lhs_val < rhs_val)
            return bigint::CmpRes(bigint::BELOW | bigint::LESS);
    }

    auto lhs_val = lhs[0];
    if(lhs_val > rhs) return bigint::CmpRes(bigint::ABOVE | bigint::GREATER);
    else if(lhs_val < rhs) return bigint::CmpRes(bigint::BELOW | bigint::LESS);

    return bigint::EQUAL;
}

bigint::CmpRes bigint::cmp(const bigint& other) const {
    inr_assert(bits_ == other.bits_, "bigint cmp(): bit count does not match");

    if(isOnHeap()) {
        return compare_two_limbarr(heapStorage_, other.heapStorage_,
                                   getLimbCount(), bits_);
    }
    else {
        return compare_two_limbs(inlineStorage_, other.inlineStorage_, bits_);
    }
}

bigint::CmpRes bigint::cmp(Limb val, bool signExt) const {
    if(isOnHeap()) {
        return compare_two_limbs_arr_signext(heapStorage_, val, getLimbCount(),
                                             bits_, signExt);
    }
    else {
        if(bits_ < bigint::LIMB_BITS) {
            val &= (bigint::Limb(1) << bits_) - 1;
        }
        return compare_two_limbs(inlineStorage_, val, bits_);
    }
}

void bigint::shr(const bigint& other) {
    inr_assert(bits_ == other.bits_, "bigint shr(): bit count does not match");
    if(other >= bits_) {
        clearBits();
        return;
    }
    shr(*other.getData());
}

bool bigint::operator>(const bigint& other) const {
    return cmp(other) & ABOVE;
}

bool bigint::operator>(Limb val) const {
    return cmp(val) & ABOVE;
}

bool bigint::operator>=(const bigint& other) const {
    return cmp(other) & (ABOVE | EQUAL);
}

bool bigint::operator>=(Limb val) const {
    return cmp(val) & (ABOVE | EQUAL);
}

bool bigint::operator<(const bigint& other) const {
    return cmp(other) & BELOW;
}

bool bigint::operator<(Limb val) const {
    return cmp(val) & BELOW;
}

bool bigint::operator<=(const bigint& other) const {
    return cmp(other) & (BELOW | EQUAL);
}

bool bigint::operator<=(Limb val) const {
    return cmp(val) & (BELOW | EQUAL);
}

void bigint::bitAnd(const bigint& other) {
    inr_assert(bits_ == other.bits_,
               "bigint bitAnd(): bit count does not match");
    if(isOnHeap()) {
        for(unsigned i = 0; i < getLimbCount(); i++) {
            heapStorage_[i] &= other.heapStorage_[i];
        }
    }
    else inlineStorage_ &= other.inlineStorage_;
    clearTopBits();
}

void bigint::bitOr(const bigint& other) {
    inr_assert(bits_ == other.bits_,
               "bigint bitOr(): bit count does not match");
    if(isOnHeap()) {
        for(unsigned i = 0; i < getLimbCount(); i++) {
            heapStorage_[i] |= other.heapStorage_[i];
        }
    }
    else inlineStorage_ |= other.inlineStorage_;
    clearTopBits();
}

void bigint::bitXor(const bigint& other) {
    inr_assert(bits_ == other.bits_,
               "bigint bitXor(): bit count does not match");
    if(isOnHeap()) {
        for(unsigned i = 0; i < getLimbCount(); i++) {
            heapStorage_[i] ^= other.heapStorage_[i];
        }
    }
    else inlineStorage_ ^= other.inlineStorage_;
    clearTopBits();
}

void bigint::bitAnd(Limb val, bool signExt) {
    if(isOnHeap()) {
        signExt = signExt && (val & (Limb(1) << (LIMB_BITS - 1)));
        heapStorage_[0] &= val;
        if(!signExt) {
            std::memset(heapStorage_ + 1, 0,
                        (getLimbCount() - 1) * sizeof(Limb));
        }
    }
    else inlineStorage_ &= val;
    clearTopBits();
}

void bigint::bitOr(Limb val, bool signExt) {
    if(isOnHeap()) {
        signExt = signExt && (val & (Limb(1) << (LIMB_BITS - 1)));
        heapStorage_[0] |= val;
        if(signExt) {
            std::memset(heapStorage_ + 1, -1,
                        (getLimbCount() - 1) * sizeof(Limb));
        }
    }
    else inlineStorage_ |= val;
    clearTopBits();
}

void bigint::bitXor(Limb val, bool signExt) {
    if(isOnHeap()) {
        signExt = signExt && (val & (Limb(1) << (LIMB_BITS - 1)));
        heapStorage_[0] ^= val;
        if(signExt) {
            for(unsigned i = 1; i < getLimbCount(); i++) {
                heapStorage_[i] ^= Limb(-1);
            }
        }
    }
    else inlineStorage_ ^= val;
    clearTopBits();
}

bigint& bigint::operator&=(const bigint& other) {
    bitAnd(other);
    return *this;
}

bigint bigint::operator&(const bigint& other) const {
    bigint cpy(*this);
    cpy.bitAnd(other);
    return cpy;
}

bigint& bigint::operator|=(const bigint& other) {
    bitOr(other);
    return *this;
}

bigint bigint::operator|(const bigint& other) const {
    bigint cpy(*this);
    cpy.bitOr(other);
    return cpy;
}

bigint& bigint::operator^=(const bigint& other) {
    bitXor(other);
    return *this;
}

bigint bigint::operator^(const bigint& other) const {
    bigint cpy(*this);
    cpy.bitXor(other);
    return cpy;
}

unsigned bigint::getEffectiveBitWidth(bool isSigned) const {
    isSigned = isSigned && getSignBit();

    if(!isSigned) {
        return bits_ - countlz();
    }
    else return bits_ - countlo() + 1;
}

} // namespace inr
