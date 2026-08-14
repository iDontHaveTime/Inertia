// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_ADT_IVECTOR_H
#define INERTIA_ADT_IVECTOR_H

/// @file ADT/IVector.h
/// @brief Provides a vector class that can store some elements on stack.

#include <inr/Support/Assert.h>

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <vector>

namespace inr {

/// @brief A vector class that can hold N elements on the stack before
/// allocating heap memory.
/// @note Not exception safe.
template<typename T, std::size_t N>
class ivec {
public:
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

private:
    alignas(value_type) std::byte inlineStorage_[N * sizeof(value_type)];
    size_type size_ = 0;
    size_type capacity_ = N;
    pointer data_ = (pointer)inlineStorage_;

    bool isOnHeap() const {
        return data_ != (const_pointer)inlineStorage_;
    }

    void freeMemory() {
        clear();
        if(isOnHeap()) {
            ::operator delete[](data_, std::align_val_t(alignof(value_type)));
            data_ = (pointer)inlineStorage_;
            capacity_ = N;
        }
    }

    constexpr static size_type DEFAULT_GROW_MINIMUM = 0x10;

    void growMemory(size_type minCapacity) {
        if(minCapacity <= capacity_) return;

        size_type newCapacity = std::bit_ceil(
            std::max<size_type>(minCapacity, DEFAULT_GROW_MINIMUM));
        pointer newData =
            (pointer)::operator new[](newCapacity * sizeof(value_type),
                                      std::align_val_t(alignof(value_type)));

        if constexpr(std::is_trivially_copyable_v<value_type>) {
            std::memcpy(newData, data_, size_ * sizeof(value_type));
        }
        else {
            for(size_type i = 0; i < size_; i++) {
                new(newData + i) value_type(std::move(data_[i]));
                data_[i].~value_type();
            }
        }

        if(isOnHeap()) {
            ::operator delete[](data_, std::align_val_t(alignof(value_type)));
        }

        data_ = newData;
        capacity_ = newCapacity;
    }

public:
    /// @brief Creates an empty vector.
    /// @note Does not initialize elements on stack.
    ivec() = default;

    ivec(const ivec& other) {
        insert(begin(), other.begin(), other.end());
    }

    ivec& operator=(const ivec& other) {
        if(this != &other) {
            freeMemory(), insert(begin(), other.begin(), other.end());
        }
        return *this;
    }

    /// @brief Move constructor.
    ivec(ivec&& other) noexcept {
        size_ = other.size_;
        capacity_ = other.capacity_;

        if(other.isOnHeap()) {
            data_ = other.data_;
        }
        else {
            data_ = (pointer)inlineStorage_;
            if constexpr(std::is_trivially_copyable_v<value_type>) {
                std::memcpy(data_, other.data_, size_ * sizeof(value_type));
            }
            else {
                for(size_type i = 0; i < size_; i++) {
                    new(data_ + i) value_type(std::move(other.data_[i]));
                    other.data_[i].~value_type();
                }
            }
        }
        other.data_ = (pointer)other.inlineStorage_;
        other.size_ = 0;
        other.capacity_ = N;
    }

    /// @brief Move operator.
    ivec& operator=(ivec&& other) noexcept {
        if(this == &other) return *this;

        freeMemory();

        size_ = other.size_;
        capacity_ = other.capacity_;

        if(other.isOnHeap()) {
            data_ = other.data_;
        }
        else {
            data_ = (pointer)inlineStorage_;
            if constexpr(std::is_trivially_copyable_v<value_type>) {
                std::memcpy(data_, other.data_, size_ * sizeof(value_type));
            }
            else {
                for(size_type i = 0; i < size_; i++) {
                    new(data_ + i) value_type(std::move(other.data_[i]));
                    other.data_[i].~value_type();
                }
            }
        }

        other.data_ = (pointer)other.inlineStorage_;
        other.size_ = 0;
        other.capacity_ = N;

        return *this;
    }

    ~ivec() {
        freeMemory();
    }

    ivec(std::initializer_list<value_type> init) {
        insert(begin(), init.begin(), init.end());
    }

    ivec& operator=(std::initializer_list<value_type> init) {
        freeMemory();
        insert(begin(), init.begin(), init.end());
        return *this;
    }

    /// @brief Destructs and sets the size back to 0.
    /// @note Does not free memory nor goes back to stack.
    void clear() {
        if constexpr(!std::is_trivially_destructible_v<value_type>) {
            while(size_) {
                data_[--size_].~value_type();
            }
        }
        else {
            size_ = 0;
        }
    }

    /// @brief Returns a pointer to the data.
    pointer data() {
        return data_;
    }

    /// @brief Returns a const pointer to the data.
    const_pointer data() const {
        return data_;
    }

    /// @brief Accesses an element.
    reference operator[](size_type n) {
        inr_assert(n < size_, "ivec operator[]: out of bounds");
        return data_[n];
    }

    /// @brief Accesses an element.
    const_reference operator[](size_type n) const {
        inr_assert(n < size_, "ivec operator[] (const): out of bounds");
        return data_[n];
    }

    /// @brief Alias for operator[].
    /// @note Does not throw exceptions like STL's at().
    reference at(size_type n) {
        inr_assert(n < size_, "ivec at(): out of bounds");
        return data_[n];
    }

    /// @brief Alias for const operator[].
    /// @note Does not throw exceptions like STL's at();
    const_reference at(size_type n) const {
        inr_assert(n < size_, "ivec at() (const): out of bounds");
        return data_[n];
    }

    /// @brief Constructs another element at the specified position.
    /// @return Iterator to the slot it was placed in.
    template<typename... Args>
    iterator emplace(iterator pos, Args&&... args) {
        size_type index = pos - data_;
        inr_assert(index <= size_, "ivec emplace(): position is out of range");

        growMemory(size_ + 1);

        for(size_type i = size_; i > index; i--) {
            new(data_ + i) value_type(std::move(data_[i - 1]));
            data_[i - 1].~value_type();
        }

        new(data_ + index) value_type(std::forward<Args>(args)...);
        size_++;
        return data_ + index;
    }

    template<typename... Args>
    reference emplace_back(Args&&... args) {
        return *emplace(end(), std::forward<Args>(args)...);
    }

    reference push_back(const_reference v) {
        return emplace_back(v);
    }

    reference push_back(value_type&& v) {
        return emplace_back(std::move(v));
    }

    iterator insert(iterator pos, const_reference v) {
        return emplace(pos, v);
    }

    iterator insert(iterator pos, value_type&& v) {
        return emplace(pos, std::move(v));
    }

    template<typename InputIt>
    iterator insert(iterator pos, InputIt first, InputIt last) {
        size_type index = pos - data_;
        size_type count = std::distance(first, last);
        if(!count) return pos;

        growMemory(size_ + count);

        if constexpr(std::is_trivially_copyable_v<value_type>) {
            std::memmove(data_ + index + count, data_ + index,
                         (size_ - index) * sizeof(value_type));
        }
        else {
            for(size_type i = size_; i > index; i--) {
                new(data_ + i + count - 1) value_type(std::move(data_[i - 1]));
                data_[i - 1].~value_type();
            }
        }

        size_type i = 0;
        for(auto it = first; it != last; ++it, i++) {
            new(data_ + index + i) value_type(*it);
        }

        size_ += count;
        return data_ + index;
    }

    void erase(iterator pos) {
        inr_assert(pos < end(), "ivec erase(): iterator out of range");

        if constexpr(!std::is_trivially_destructible_v<value_type>)
            pos->~value_type();

        if constexpr(std::is_trivially_copyable_v<value_type>) {
            if(pos + 1 != end()) {
                std::memmove(pos, pos + 1, (end() - pos) * sizeof(value_type));
            }
        }
        else {
            for(pos++; pos < end(); pos++) {
                new(pos - 1) value_type(std::move(*pos));
                pos->~value_type();
            }
        }
        size_--;
    }

    bool erase_if(iterator pos, bool cond) {
        if(cond) {
            erase(pos);
            return true;
        }
        return false;
    }

    bool erase_if_found(const_reference v) {
        auto it = find(v);
        return erase_if(it, it != end());
    }

    /// @brief Returns the amount of elements in the vector.
    size_type size() const {
        return size_;
    }

    /// @brief Returns the max capacity of the vector at the moment.
    size_type capacity() const {
        return capacity_;
    }

    /// @brief Returns whether the vector is empty or not.
    bool empty() const {
        return size_ == 0;
    }

    iterator begin() {
        return data_;
    }

    iterator end() {
        return data_ + size_;
    }

    const_iterator begin() const {
        return data_;
    }

    const_iterator end() const {
        return data_ + size_;
    }

    const_iterator cbegin() const {
        return begin();
    }

    const_iterator cend() const {
        return end();
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const {
        return rbegin();
    }
    const_reverse_iterator crend() const {
        return rend();
    }

    /// @brief Returns this as an STL vector.
    std::vector<value_type> vec() {
        return std::vector<value_type>(begin(), end());
    }

    /// @brief Compares elements of the two vectors.
    bool operator==(const ivec& other) const {
        return std::equal(begin(), end(), other.begin(), other.end());
    }

    /// @brief Returns the first element.
    reference front() {
        inr_assert(size_ != 0, "ivec front(): size was zero");
        return data_[0];
    }

    /// @brief Returns the last element.
    reference back() {
        inr_assert(size_ != 0, "ivec back(): size was zero");
        return data_[size_ - 1];
    }

    /// @brief Returns the first element, const.
    const_reference front() const {
        inr_assert(size_ != 0, "ivec front() (const): size was zero");
        return data_[0];
    }

    /// @brief Returns the last element, const.
    const_reference back() const {
        inr_assert(size_ != 0, "ivec back() (const): size was zero");
        return data_[size_ - 1];
    }

    void pop_back() {
        inr_assert(size_ != 0, "ivec pop_back(): size was zero");
        if constexpr(!std::is_trivially_destructible_v<value_type>)
            data_[size_ - 1].~value_type();
        size_--;
    }

    const_iterator find(const_reference val) const {
        return std::find(begin(), end(), val);
    }

    iterator find(const_reference val) {
        return std::find(begin(), end(), val);
    }
};

} // namespace inr

#endif // INERTIA_ADT_IVECTOR_H
