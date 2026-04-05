// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_ADT_BIGINT_H
#define INERTIA_ADT_BIGINT_H

/// @file ADT/BigInt.h
/// @brief Provides an arbitrary precision integer class.

#include <inr/ADT/ArrView.h>
#include <inr/Support/Stream.h>

#include <bit>
#include <climits>
#include <cstddef>
#include <stdexcept>

namespace inr {

/// @brief Arbitrary precision integer.
///
/// Uses `unsigned long long` for limbs.
/// Has inline optimization (being on stack).
class bigint {
public:
    /// @brief Unsigned limb.
    using Limb = unsigned long long;
    /// @brief Signed limb.
    using SLimb = long long;

    /// @brief How many bits in a limb.
    constexpr static size_t LIMB_BITS = sizeof(Limb) * CHAR_BIT;
    /// @brief Count zeroes from the LSB to MSB of the bits.
    constexpr static size_t LIMB_DIV = std::countr_zero(LIMB_BITS);

private:
    union {
        Limb stack_; ///< When bits <= LIMB_BITS.
        Limb* heap_; ///< Fallback to heap when too many bits.
    };
    size_t bits_; ///< How many bits is this bigint.

    /// @brief Calculates the amount of limbs it has allocated.
    inline size_t calculateLimbC() const noexcept {
        return (bits_ + (LIMB_BITS - 1)) >> LIMB_DIV;
    }

    /// @brief Resize the integer.
    void setNewSize() {
        if(bits_ <= LIMB_BITS) {
            return;
        }
        heap_ = new Limb[calculateLimbC()];
    }

    /// @brief Returns one past the last limb.
    Limb* heapEnd() noexcept {
        return heap_ + calculateLimbC();
    }

    /// @brief Returns the last limb.
    Limb* heapLast() noexcept {
        return (heap_ + ((bits_ - 1) >> LIMB_DIV));
    }

    /// @brief Const version of heapLast().
    const Limb* heapLast() const noexcept {
        return (heap_ + ((bits_ - 1) >> LIMB_DIV));
    }

    /// @brief Copies the last limb, both stack and heap.
    Limb last() const noexcept {
        if(onStack()) return stack_;
        return *heapLast();
    }

    /// @brief Clear the top bits.
    bigint& zeroOutTopBits() noexcept {
        if(!(bits_ & ((2 << LIMB_DIV) - 1))) return *this;

        if(onStack()) {
            stack_ &= (Limb(1) << bits_) - 1;
        }
        else {
            *heapLast() &= (Limb(1) << bits_) - 1;
        }
        return *this;
    }

public:
    /// @brief Constructs a bigint with bit width of 1 and value of 0.
    bigint() noexcept : stack_(0), bits_(1) {}

    /// @brief Basic constructor for bigint.
    /// @param bits Width of the integer.
    explicit bigint(size_t bits) : bits_(bits) {
        setNewSize();
    }

    /// @brief Constructs a bigint and sets the value of it based on the limbs
    /// given.
    /// @param bits Width of the integer.
    /// @param limbs Limbs, most significant limb first.
    bigint(size_t bits, arrview<Limb> limbs) : bigint(bits) {
        if(onStack()) {
            Limb val = limbs.back();
            val &= (Limb(1) << bits) - 1;

            stack_ = val;
            return;
        }
        Limb* i = heap_;
        for(auto it = limbs.rbegin(); it != limbs.rend(); ++it) {
            if(i == heapEnd()) return;
            *i = *it;
        }
    }

    /// @brief Constructs a bigint from the Limb value provided.
    /// @param bits Width of the integer.
    /// @param val Value for the limb.
    /// @param isSigned Should it sign extend.
    bigint(size_t bits, Limb val, bool isSigned = false) : bigint(bits) {
        if(onStack()) {
            val &= (Limb(1) << bits) - 1;

            if(isSigned && bits > 0) {
                Limb sign_bit = Limb(1) << (bits - 1);
                if(val & sign_bit) {
                    val |= ~((sign_bit << 1) - 1);
                }
            }

            stack_ = val;
            return;
        }
        *heap_ = val;

        Limb ext = (isSigned && (val >> ((1 << LIMB_DIV) - 1))) ? ~Limb(0) : 0;
        std::fill(heap_ + 1, heapEnd(), ext);
    }

    /// @brief Copy constructor.
    bigint(const bigint& other) : bits_(other.bits_) {
        if(onStack()) {
            stack_ = other.stack_;
            return;
        }
        setNewSize();
        for(size_t i = 0; i < calculateLimbC(); i++) {
            heap_[i] = other.heap_[i];
        }
    }

    /// @brief Copy operator.
    bigint& operator=(const bigint& other) {
        if(this != &other) {
            if(onHeap()) delete[] heap_;
            bits_ = other.bits_;
            setNewSize();
            if(onStack()) {
                stack_ = other.stack_;
            }
            else {
                for(size_t i = 0; i < calculateLimbC(); i++) {
                    heap_[i] = other.heap_[i];
                }
            }
        }
        return *this;
    }

    /// @brief Move constructor.
    bigint(bigint&& other) noexcept : bits_(other.bits_) {
        if(onStack()) {
            stack_ = other.stack_;
        }
        else {
            heap_ = other.heap_;
            other.heap_ = nullptr;
        }

        other.bits_ = 0;
    }

    /// @brief Move operator.
    bigint& operator=(bigint&& other) noexcept {
        if(this != &other) {
            if(onHeap()) delete[] heap_;
            bits_ = other.bits_;
            if(onStack()) {
                stack_ = other.stack_;
            }
            else {
                heap_ = other.heap_;
                other.heap_ = nullptr;
            }

            other.bits_ = 0;
        }
        return *this;
    }

    /// @brief Pointer to the first limb.
    const Limb* data() const noexcept {
        return bits_ > LIMB_BITS ? heap_ : &stack_;
    }

    /// @brief Is this bigint on heap or not.
    bool onHeap() const noexcept {
        return bits_ > LIMB_BITS;
    }

    /// @brief Is this bigint on stack or not.
    bool onStack() const noexcept {
        return bits_ <= LIMB_BITS;
    }

    ~bigint() noexcept {
        if(onHeap()) delete[] heap_;
    }

    /// @brief Returns the amount of Limb limbs.
    size_t size() const noexcept {
        return calculateLimbC();
    }

    /// @brief Is the value zero.
    bool isZero() const noexcept {
        if(onStack()) return !stack_;
        for(size_t i = 0; i < size(); i++) {
            if(heap_[i] != 0) return false;
        }
        return true;
    }

    /// @brief Returns whether or not a bit is set.
    /// @param bit Bit, starts from 0.
    /// @throws std::out_of_range() if the bit is out of range.
    ///
    /// For example if this bigint has 65 bits, bit 65 would be: `getBit(64)`.
    bool getBit(size_t bit) const {
        if(bit >= bits_)
            throw std::out_of_range("The selected bit is out of range.");
        if(onStack()) {
            return stack_ & (Limb(1) << bit);
        }
        size_t limb = bit >> LIMB_DIV;
        size_t bitInLimb = bit & ((1 << LIMB_DIV) - 1);

        return (heap_[limb] >> bitInLimb) & 1;
    }

    /// @brief Gets the sign bit.
    bool getSign() const noexcept {
        return getBit(bits_ - 1);
    }

    /// @brief Flips all bits, just the ~ operator.
    void flipAllBits() {
        if(onStack()) {
            stack_ = ~stack_;
        }
        else {
            for(size_t i = 0; i < size(); i++) {
                heap_[i] = ~heap_[i];
            }
        }
        zeroOutTopBits();
    }

    /// @brief Returns the value as a string.
    /// @see print() for explanation of the args.
    std::string toString(unsigned radix, bool isSigned, bool addPrefix,
                         bool upperCase) const {
        string_stream ss;
        print(ss, radix, isSigned, addPrefix, upperCase);
        return ss.str();
    }

    /// @brief Prints out the value of this bigint to a stream.
    /// @param radix Radix, aka base (e.g. 2 binary, 8 octal, 10 decimal,
    /// etc..).
    /// @param isSigned Is this bigint signed.
    /// @param addPrefix Should this add a prefix (e.g. 0b, 0x, etc..).
    /// @param upperCase Should the letters be uppercase (applies to radix 16).
    void print(raw_stream&, unsigned radix, bool isSigned, bool addPrefix,
               bool upperCase) const;

    friend raw_stream& operator<<(raw_stream& os, const bigint& bi) {
        bi.print(os, 10, false, true, true);
        return os;
    }

private:
    /// @brief Checks whether or not bits match.
    void checkBitWidths(const bigint& other) const {
        if(bits_ != other.bits_)
            throw std::runtime_error("bigint bits must match in operators");
    }

    static bool bigintAdd(Limb* dest, const Limb* src, bool c,
                          size_t limbs) noexcept;
    static bool bigintAddLimb(Limb* dest, Limb src, size_t limbs) noexcept;
    static void bigintShiftRight(Limb* dest, size_t limbs,
                                 unsigned shiftN) noexcept;
    static void base10Impl(bigint& tmp, raw_stream&);

public:
    /// @brief Shifts the bits right N amount of times.
    /// @param times N.
    void shiftRight(unsigned times) noexcept {
        if(onStack()) {
            stack_ >>= times;
        }
        else {
            bigintShiftRight(heap_, size(), times);
        }
    }

    /// @brief Compares the values of two bigints.
    bool operator==(const bigint& other) const {
        checkBitWidths(other);

        if(onStack()) {
            return stack_ == other.stack_;
        }

        for(size_t i = 0; i < size(); i++) {
            if(heap_[i] != other.heap_[i]) return false;
        }

        return true;
    }

    /// @brief Are the values of the bigints different.
    bool operator!=(const bigint& other) const {
        return !(*this == other);
    }

    /// @brief Adds another bigint into this one.
    bigint& operator+=(const bigint&);

    /// @brief Creates a new addition result of two bigints.
    bigint operator+(const bigint& other) const {
        return bigint(*this) += other;
    }

    /// @brief Increases the bigint by one.
    bigint& operator++() noexcept {
        if(onStack()) {
            ++stack_;
        }
        else {
            bigintAddLimb(heap_, 1, size());
        }
        return zeroOutTopBits();
    }

    /// @brief Increases the bigint by one and returns a copy.
    bigint operator++(int) {
        bigint cpy(*this);
        ++*this;
        return cpy;
    }

    /// @brief Same as -N.
    void negate() {
        flipAllBits();
        ++*this;
    }
};

} // namespace inr

#endif // INERTIA_ADT_BIGINT_H
