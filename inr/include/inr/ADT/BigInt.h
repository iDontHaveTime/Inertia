// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_ADT_BIGINT_H
#define INERTIA_ADT_BIGINT_H

/// @file ADT/BigInt.h
/// @brief Provides an arbitrary precision integer class.

#include <inr/ADT/ArrView.h>
#include <inr/Support/Stream.h>

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace inr {

class bigint {
    union {
        uint64_t stack_;
        uint64_t* heap_;
    };
    size_t bits_;

    inline size_t calculateLimbC() const noexcept {
        return (bits_ + 63) >> 6;
    }

    void setNewSize() {
        if(bits_ <= 64) {
            return;
        }
        heap_ = new uint64_t[calculateLimbC()];
    }

    uint64_t* heapEnd() noexcept {
        return heap_ + calculateLimbC();
    }

    uint64_t* heapLast() noexcept {
        return (heap_ + ((bits_ - 1) >> 6));
    }

    const uint64_t* heapLast() const noexcept {
        return (heap_ + ((bits_ - 1) >> 6));
    }

    uint64_t last() const noexcept {
        if(onStack()) return stack_;
        return *heapLast();
    }

    bigint& zeroOutTopBits() noexcept {
        if(!(bits_ & 127)) return *this;

        if(onStack()) {
            stack_ &= (uint64_t(1) << bits_) - 1;
        }
        else {
            *heapLast() &= (uint64_t(1) << bits_) - 1;
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
    bigint(size_t bits, arrview<uint64_t> limbs) : bigint(bits) {
        if(onStack()) {
            uint64_t val = limbs.back();
            val &= (uint64_t(1) << bits) - 1;

            stack_ = val;
            return;
        }
        uint64_t* i = heap_;
        for(auto it = limbs.rbegin(); it != limbs.rend(); ++it) {
            if(i == heapEnd()) return;
            *i = *it;
        }
    }

    /// @brief Constructs a bigint from the uint64_t value provided.
    /// @param bits Width of the integer.
    /// @param val Value for the limb.
    /// @param isSigned Should it sign extend.
    bigint(size_t bits, uint64_t val, bool isSigned = false) : bigint(bits) {
        if(onStack()) {
            val &= (uint64_t(1) << bits) - 1;

            if(isSigned && bits > 0) {
                uint64_t sign_bit = uint64_t(1) << (bits - 1);
                if(val & sign_bit) {
                    val |= ~((sign_bit << 1) - 1);
                }
            }

            stack_ = val;
            return;
        }
        *heap_ = val;

        uint64_t ext = (isSigned && (val >> 63)) ? ~uint64_t(0) : 0;
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

    const uint64_t* data() const noexcept {
        return bits_ > 64 ? heap_ : &stack_;
    }

    /// @brief Is this bigint on heap or not.
    bool onHeap() const noexcept {
        return bits_ > 64;
    }

    /// @brief Is this bigint on stack or not.
    bool onStack() const noexcept {
        return bits_ <= 64;
    }

    ~bigint() noexcept {
        if(onHeap()) delete[] heap_;
    }

    /// @brief Returns the amount of uint64_t limbs.
    size_t size() const noexcept {
        return calculateLimbC();
    }

    bool isZero() const noexcept {
        if(onStack()) return !stack_;
        for(size_t i = 0; i < size(); i++) {
            if(heap_[i] != 0) return false;
        }
        return true;
    }

    bool getBit(size_t bit) const {
        if(bit >= bits_)
            throw std::out_of_range("The selected bit is out of range.");
        if(onStack()) {
            return stack_ & (uint64_t(1) << bit);
        }
        size_t limb = bit >> 6;
        size_t bitInLimb = bit & 63;

        return (heap_[limb] >> bitInLimb) & 1;
    }

    bool getSign() const noexcept {
        return getBit(bits_ - 1);
    }

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

    std::string toString(unsigned radix, bool isSigned, bool addPrefix,
                         bool upperCase) const {
        string_stream ss;
        print(ss, radix, isSigned, addPrefix, upperCase);
        return ss.str();
    }

    void print(raw_stream&, unsigned radix, bool isSigned, bool addPrefix,
               bool upperCase) const;

    friend raw_stream& operator<<(raw_stream& os, const bigint& bi) {
        bi.print(os, 10, false, true, true);
        return os;
    }

private:
    void checkBitWidths(const bigint& other) const {
        if(bits_ != other.bits_)
            throw std::runtime_error("bigint bits must match in operators");
    }

    static bool bigintAdd(uint64_t* dest, const uint64_t* src, bool c,
                          size_t limbs) noexcept;
    static bool bigintAddLimb(uint64_t* dest, uint64_t src,
                              size_t limbs) noexcept;
    static void bigintShiftRight(uint64_t* dest, size_t limbs,
                                 unsigned shiftN) noexcept;

public:
    void shiftRight(unsigned times) noexcept {
        if(onStack()) {
            stack_ >>= times;
        }
        else {
            bigintShiftRight(heap_, size(), times);
        }
    }

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

    bool operator!=(const bigint& other) const {
        return !(*this == other);
    }

    bigint& operator+=(const bigint&);

    bigint operator+(const bigint& other) const {
        return bigint(*this) += other;
    }

    bigint& operator++() noexcept {
        if(onStack()) {
            ++stack_;
        }
        else {
            bigintAddLimb(heap_, 1, size());
        }
        return zeroOutTopBits();
    }

    bigint operator++(int) {
        bigint cpy(*this);
        ++*this;
        return cpy;
    }

    void negate() {
        flipAllBits();
        ++*this;
    }
};

} // namespace inr

#endif // INERTIA_ADT_BIGINT_H
