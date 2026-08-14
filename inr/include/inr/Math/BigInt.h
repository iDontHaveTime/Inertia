// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_MATH_BIGINT_H
#define INERTIA_MATH_BIGINT_H

/// @file Math/BigInt.h
/// @brief Provides a class that can store arbitrary precision integers.

#include <climits>
#include <cstdint>

namespace inr {

class stream;

/// @brief Stores an integer number with any number of bits.
/// @note All operation must be done on same bit sized bigints.
class bigint {
public:
    using Limb = uint64_t;
    constexpr static unsigned LIMB_BITS = sizeof(Limb) * CHAR_BIT;

    enum CmpRes : unsigned {
        EQUAL = 0x1,   ///< Two values are equal.
        ABOVE = 0x2,   ///< Unsigned above.
        BELOW = 0x4,   ///< Unsigned below.
        GREATER = 0x8, ///< Signed greater.
        LESS = 0x10,   ///< Signed less.
    };

private:
    union {
        Limb inlineStorage_;
        Limb* heapStorage_;
    };
    unsigned bits_ = 0;

    /// @brief Allocates the amount of bits specified.
    /// @return Number of limbs allocated.
    /// @note The memory is zeroed out.
    unsigned allocateNewStorage(unsigned bits);
    /// @brief Resizes the storage thus keeping the current value.
    void resizeStorage(unsigned bits, bool signExt);
    void freeStorage();
    Limb* getLastHeapLimb();
    const Limb* getLastHeapLimb() const;

    const Limb* getData() const {
        return isOnHeap() ? heapStorage_ : &inlineStorage_;
    }

    Limb* getData() {
        return isOnHeap() ? heapStorage_ : &inlineStorage_;
    }

    bool isOnHeap() const {
        return bits_ > LIMB_BITS;
    }

    /// @brief Clears out the top bits that are not used.
    void clearTopBits();

    static bool biaddimpl(Limb* dest, const Limb* src, bool c, unsigned limbc);
    static bool biaddimpllimb(Limb* dest, Limb src, unsigned limbc);

    static bool bisubimpl(Limb* dest, const Limb* src, bool b, unsigned limbc);
    static bool bisubimpllimb(Limb* dest, Limb src, unsigned limbc);

    static void bishrimpl(Limb* dest, unsigned limbc, unsigned shiftN);

    static void bib10impl(const bigint& tmp, stream& os);

public:
    /// @brief Creates a bigint with 1 bit zeroed out.
    bigint();

    /// @brief Creates a bigint with the provided number of bits zeroed out.
    explicit bigint(unsigned bits);

    bigint(unsigned bits, Limb val, bool signExt = false);

    bigint(const bigint&);
    bigint& operator=(const bigint&);

    bigint(bigint&&) noexcept;
    bigint& operator=(bigint&&) noexcept;

    ~bigint() {
        freeStorage();
    }

    /// @brief Unary plus operator.
    bigint operator+() const {
        return *this;
    }

    /// @brief Returns true if its zero.
    bool operator!() const {
        return isZero();
    }

    /// @brief Returns the amount of bits of this bigint.
    unsigned getBits() const {
        return bits_;
    }

    /// @brief Returns the amount of bits are actually in memory.
    unsigned getAllocatedBits() const {
        return (bits_ + (LIMB_BITS - 1)) & ~(LIMB_BITS - 1);
    }

    /// @brief Returns how much limbs are in this bigint.
    unsigned getLimbCount() const {
        return isOnHeap() ? (getAllocatedBits() / LIMB_BITS) : 1;
    }

    /// @brief Returns the bit in the index given.
    /// @note The index spans from 0 to bits - 1.
    ///
    /// So for example if we have 64 bits, the sign bit is bit 63, and LSB is
    /// bit 0.
    bool getBit(unsigned b) const;

    /// @brief Turns the provided bit to one.
    /// @note The index spans from 0 to bits - 1.
    void setBit(unsigned b);

    /// @brief Sets all bits to 1.
    void setBits();

    /// @brief Turns the provided bit to zero.
    /// @note The index spans from 0 to bits - 1.
    void clearBit(unsigned b);

    /// @brief Sets all bits to 0.
    void clearBits();

    /// @brief Flips all bits.
    void flipBits();

    /// @brief Returns the status of the sign bit.
    bool getSignBit() const {
        return getBit(bits_ - 1);
    }

    /// @brief Sets the sign bit to 1.
    void setSignBit() {
        setBit(bits_ - 1);
    }

    /// @brief Sets the sign bit to 0.
    void clearSignBit() {
        clearBit(bits_ - 1);
    }

    /// @brief ~bigint.
    bigint operator~() const;

    /// @brief Truncates the bigint down.
    /// @note Asserts that bits are going down.
    bigint truncate(unsigned b) const;
    /// @brief Sign extend the bigint.
    /// @note Asserts that bits are going up.
    bigint signext(unsigned b) const;
    /// @brief Zero extends the bigint.
    /// @note Asserts that bits are going up.
    bigint zeroext(unsigned b) const;

    /// @brief Truncates or zero extends.
    bigint trunczero(unsigned b) const;
    /// @brief Truncates or sign extends.
    bigint truncsign(unsigned b) const;

    /// @brief Returns true if the bigint is zero or not.
    bool isZero() const;

    /// @brief Prints out the value of this bigint to a stream.
    /// @param radix Radix, aka base (e.g. 2 binary, 8 octal, 10 decimal,
    /// etc..).
    /// @param isSigned Is this bigint signed.
    /// @param addPrefix Should this add a prefix (e.g. 0b, 0x, etc..).
    /// @param upperCase Should the letters be uppercase (applies to radix 16).
    void print(stream& os, unsigned radix, bool isSigned, bool addPrefix,
               bool upperCase) const;

    /// @brief Prints out the bigint as base 10 signed one.
    friend stream& operator<<(stream&, const bigint&);

    /// @brief Shifts right n amount of times.
    void shr(unsigned n);

    /// @brief Shifts the bigint to the right.
    /// @note Both bigints must be equal bitwidth.
    void shr(const bigint& other);

    /// @brief bigint >>= unsigned.
    bigint& operator>>=(unsigned n);

    /// @brief bigint >> unsigned.
    bigint operator>>(unsigned n) const;

    /// @brief bigint >>= bigint.
    bigint& operator>>=(const bigint& other);

    /// @brief bigint >> bigint.
    bigint operator>>(const bigint& other) const;

    /// @brief Adds a uint64_t (Limb) value to the bigint.
    /// @return Carry flag.
    bool add(Limb val);

    /// @brief Adds another bigint to this one.
    /// @note Both must be the same bitwidth.
    /// @return Carry flag.
    bool add(const bigint& other);

    /// @brief Subtracts a uint64_t (Limb) value from the bigint.
    /// @return Borrow.
    bool sub(Limb val);

    /// @brief Subtracts another bigint from this one.
    /// @note Both must be the same bitwidth.
    /// @return Borrow.
    bool sub(const bigint& other);

    /// @brief bigint += uint64_t.
    bigint& operator+=(Limb val);

    /// @brief bigint += bigint.
    bigint& operator+=(const bigint& other);

    /// @brief bigint + uint64_t.
    bigint operator+(Limb val) const;

    /// @brief bigint + bigint.
    bigint operator+(const bigint& other) const;

    /// @brief ++bigint.
    bigint& operator++();

    /// @brief bigint++.
    bigint operator++(int);

    /// @brief bigint -= uint64_t.
    bigint& operator-=(Limb val);

    /// @brief bigint -= bigint.
    bigint& operator-=(const bigint& other);

    /// @brief bigint - uint64_t.
    bigint operator-(Limb val) const;

    /// @brief bigint - bigint.
    bigint operator-(const bigint& other) const;

    /// @brief --bigint.
    bigint& operator--();

    /// @brief bigint--.
    bigint operator--(int);

    /// @brief Negates this bigint, like the unary operator '-'.
    void negate();

    /// @brief -bigint.
    bigint operator-() const;

    /// @brief Returns the amount of bits that are set to 1.
    unsigned popcount() const;

    /// @brief Returns whether or not the integer is a power of two.
    bool isPow2() const {
        return popcount() == 1;
    }

    /// @brief Count leading zeros.
    unsigned countlz() const;
    /// @brief Count leading ones.
    unsigned countlo() const;
    /// @brief Count zero bits from the LSB.
    unsigned countrz() const;
    /// @brief Count one bits from the LSB.
    unsigned countro() const;

    /// @brief Compares two bigints and returns a comparison result.
    /// @note Must be equal bitwidths.
    CmpRes cmp(const bigint& other) const;

    /// @brief Compares a bigint to a uint64_t and returns a comparison result.
    /// @note signExt only works when the bigint is above 64 bits.
    CmpRes cmp(Limb val, bool signExt = false) const;

    /// @brief bigint == bigint.
    bool operator==(const bigint& other) const;
    /// @brief bigint == uint64_t, unsigned.
    bool operator==(Limb val) const;

    /// @brief bigint != bigint.
    bool operator!=(const bigint& other) const;
    /// @brief bigint != uint64_t, unsigned.
    bool operator!=(Limb val) const;

    /// @brief bigint > bigint, unsigned.
    bool operator>(const bigint& other) const;
    /// @brief bigint > uint64_t, unsigned.
    bool operator>(Limb val) const;

    /// @brief bigint >= bigint, unsigned.
    bool operator>=(const bigint& other) const;
    /// @brief bigint >= uint64_t, unsigned.
    bool operator>=(Limb val) const;

    /// @brief bigint < bigint, unsigned.
    bool operator<(const bigint& other) const;
    /// @brief bigint < uint64_t, unsigned.
    bool operator<(Limb val) const;

    /// @brief bigint <= bigint, unsigned.
    bool operator<=(const bigint& other) const;
    /// @brief bigint <= uint64_t, unsigned.
    bool operator<=(Limb val) const;

    /// @brief Bitwise AND.
    void bitAnd(const bigint& other);
    /// @brief Bitwise OR.
    void bitOr(const bigint& other);
    /// @brief Bitwise XOR.
    void bitXor(const bigint& other);

    /// @brief Bitwise AND, Limb.
    void bitAnd(Limb val, bool signExt);
    /// @brief Bitwise OR, Limb.
    void bitOr(Limb val, bool signExt);
    /// @brief Bitwise XOR, Limb.
    void bitXor(Limb val, bool signExt);

    /// @brief bigint &= bigint.
    bigint& operator&=(const bigint& other);
    /// @brief bigint & bigint.
    bigint operator&(const bigint& other) const;

    /// @brief bigint |= bigint.
    bigint& operator|=(const bigint& other);
    /// @brief bigint | bigint.
    bigint operator|(const bigint& other) const;

    /// @brief bigint ^= bigint.
    bigint& operator^=(const bigint& other);
    /// @brief bigint ^ bigint.
    bigint operator^(const bigint& other) const;

    /// @brief Returns the minimum bitwidth for the current integer.
    ///
    /// For example an integer like 0x00FF the minimum bitwidth needed is 8,
    /// even though the integer is 16 bits.
    unsigned getEffectiveBitWidth(bool isSigned) const;
};

} // namespace inr

#endif // INERTIA_MATH_BIGINT_H
