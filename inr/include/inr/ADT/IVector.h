// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_ADT_IVECTOR_H
#define INERTIA_ADT_IVECTOR_H

/// @file ADT/IVector.h
/// @brief Provides the inline vector class.

#include <inr/Support/Compiler.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <utility>
#include <vector>

namespace inr {

/// @brief Inline vector class stores on stack until N was reached.
template<typename T, size_t N>
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
    using size_type = size_t;
    using difference_type = ptrdiff_t;

private:
    alignas(value_type) uint8_t
        inlineStorage_[N * sizeof(value_type)]; ///< Stores N objects on stack.
    size_type size_ = 0;
    size_type capacity_ = N; ///< How much can this vector currently hold.
    pointer data_ = (pointer)inlineStorage_; ///< Points to the data, could be
                                             ///< the stack array or heap.

    bool isHeap() const noexcept {
        return data_ != (const_pointer)inlineStorage_;
    }

    /// @brief Frees the memory and goes back to inline storage.
    void freeMemory() noexcept {
        clear();
        if(isHeap()) {
            operator delete[](data_);
            data_ = (pointer)inlineStorage_;
            capacity_ = N;
        }
    }

    /// @brief Grows memory, moves to heap if needed.
    /// @param minCapacity Minimum capacity needed.
    /// @note May allocate more than minCapacity.
    void growMemory(size_type minCapacity) noexcept {
        if(minCapacity <= capacity_) return;

        size_type newCapacity =
            std::bit_ceil(std::max<size_type>(minCapacity, 16));
        pointer newData =
            (pointer) operator new[](newCapacity * sizeof(value_type));

        if constexpr(std::is_trivially_copyable_v<value_type>) {
            std::memcpy(newData, data_, size_ * sizeof(value_type));
        }
        else {
            for(size_type i = 0; i < size_; i++) {
                new(newData + i) value_type(std::move(data_[i]));
                data_[i].~value_type();
            }
        }

        if(isHeap()) {
            operator delete[](data_);
        }

        data_ = newData;
        capacity_ = newCapacity;
    }

public:
    /// @brief Default constructor, does nothing.
    ivec() noexcept = default;

    /// @brief Copy constructor.
    ivec(const ivec& other) noexcept {
        insert(begin(), other.begin(), other.end());
    }

    /// @brief Copy operator.
    ivec& operator=(const ivec& other) noexcept {
        if(this != &other) {
            freeMemory();
            insert(begin(), other.begin(), other.end());
        }
        return *this;
    }

    /// @brief Move constructor.
    ivec(ivec&& other) noexcept {
        size_ = other.size_;
        capacity_ = other.capacity_;

        if(other.isHeap()) {
            data_ = other.data_;
            other.data_ = (pointer)other.inlineStorage_;
            other.size_ = 0;
            other.capacity_ = N;
        }
        else {
            data_ = (pointer)inlineStorage_;
            if constexpr(std::is_trivially_copyable_v<value_type>) {
                std::memcpy(data_, other.data_, size_ * sizeof(value_type));
            }
            else {
                for(size_type i = 0; i < size_; ++i) {
                    new(data_ + i) value_type(std::move(other.data_[i]));
                }
            }
            other.clear();
        }
    }

    /// @brief Move operator.
    ivec& operator=(ivec&& other) noexcept {
        if(this == &other) return *this;

        freeMemory();

        size_ = other.size_;
        capacity_ = other.capacity_;

        if(other.isHeap()) {
            data_ = other.data_;
            other.data_ = (pointer)other.inlineStorage_;
            other.size_ = 0;
            other.capacity_ = N;
        }
        else {
            data_ = (pointer)inlineStorage_;
            if constexpr(std::is_trivially_copyable_v<value_type>) {
                std::memcpy(data_, other.data_, size_ * sizeof(value_type));
            }
            else {
                for(size_type i = 0; i < size_; ++i) {
                    new(data_ + i) value_type(std::move(other.data_[i]));
                }
            }
            other.clear();
        }

        return *this;
    }

    /// @brief Initializer list constructor.
    ivec(std::initializer_list<value_type> init) noexcept {
        growMemory(init.size());
        insert(begin(), init.begin(), init.end());
    }

    /// @brief Initializer list operator.
    ivec& operator=(std::initializer_list<value_type> init) noexcept {
        freeMemory();
        growMemory(init.size());
        insert(begin(), init.begin(), init.end());
        return *this;
    }

    /// @brief Destructor.
    ~ivec() noexcept {
        freeMemory();
    }

    /// @brief Destructs object and sets the size to 0.
    /// @note Does not free memory, nor go back to stack.
    void clear() noexcept {
        while(size_) {
            data_[--size_].~value_type();
        }
    }

    /// @brief Returns the pointer to data.
    /// @return Pointer to the first element.
    pointer data() noexcept {
        return data_;
    }

    /// @brief Returns a const pointer to data.
    /// @return Const pointer to the first element.
    const_pointer data() const noexcept {
        return data_;
    }

    /// @brief Safely accesses an element at an index.
    /// @param n Index to access it at.
    reference at(size_type n) noexcept {
        inr_assert(n < size_, "ivec's at() is out of range.");
        return data_[n];
    }

    /// @brief Same as `at(size_type)` but const.
    const_reference at(size_type n) const noexcept {
        inr_assert(n < size_, "ivec's at() is out of range.");
        return data_[n];
    }

    /// @brief Construct emplaces an object into the pos provided.
    /// @param pos Position to construct it at.
    /// @param args Args to pass in to the constructor.
    /// @return Slot it was placed in.
    template<typename... Args>
    iterator emplace(iterator pos, Args&&... args) noexcept {
        size_type index = pos - data_;
        inr_assert(index <= size_, "ivec's emplace position is out of range.");

        growMemory(size_ + 1);

        for(size_type i = size_; i > index; i--) {
            new(data_ + i) value_type(std::move_if_noexcept(data_[i - 1]));
            data_[i - 1].~value_type();
        }

        new(data_ + index) value_type(std::forward<Args>(args)...);
        size_++;
        return data_ + index;
    }

    /// @brief Constructs an object at the back of the vector.
    /// @param args Args to pass to the constructor.
    /// @return Reference to the new object.
    template<typename... Args>
    reference emplace_back(Args&&... args) noexcept {
        return *emplace(end(), std::forward<Args>(args)...);
    }

    /// @brief Copies the object to the back of the vector.
    /// @param v Object to copy.
    /// @return Reference to the new object.
    reference push_back(const_reference v) noexcept {
        return emplace_back(v);
    }

    /// @brief Moves the object to the back of the vector.
    /// @param v Object to move.
    /// @return Reference to the new object.
    reference push_back(value_type&& v) noexcept {
        return emplace_back(std::move(v));
    }

    /// @brief Copy inserts the object into the pos provided.
    /// @param pos The position to copy it to.
    /// @param v Object to copy.
    /// @return The slot it was placed.
    iterator insert(iterator pos, const_reference v) noexcept {
        return emplace(pos, v);
    }

    /// @brief Move inserts the object into the pos provided.
    /// @param pos The position to move it to.
    /// @param v Object to move.
    /// @return The slot it was placed.
    iterator insert(iterator pos, value_type&& v) noexcept {
        return emplace(pos, std::move(v));
    }

    /// @brief Range insert into the vector.
    /// @param pos Where to start from.
    /// @param first Pointer to the start.
    /// @param last Pointer to the end.
    /// @return Slot the first object was placed.
    template<typename InputIt>
    iterator insert(iterator pos, InputIt first, InputIt last) noexcept {
        size_type index = pos - data_;
        size_type count = std::distance(first, last);
        if(count == 0) return pos;

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

    /// @brief Returns the amount of elements in the vector.
    size_type size() const noexcept {
        return size_;
    }

    /// @brief Returns the max capacity of the vector at the moment.
    size_type capacity() const noexcept {
        return capacity_;
    }

    /// @brief Returns whether the vector is empty or not.
    bool empty() const noexcept {
        return size_ == 0;
    }

    /// @brief Returns a reference to an object at index n.
    /// @param n Object index.
    reference operator[](size_type n) noexcept {
        return data_[n];
    }

    /// @brief Returns a const reference to an object at index n.
    /// @param n Object index.
    const_reference operator[](size_type n) const noexcept {
        return data_[n];
    }

    iterator begin() noexcept {
        return data_;
    }
    iterator end() noexcept {
        return data_ + size_;
    }
    const_iterator begin() const noexcept {
        return data_;
    }
    const_iterator end() const noexcept {
        return data_ + size_;
    }
    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    /// @brief Returns this as an STL vector.
    std::vector<value_type> vec() noexcept {
        return std::vector<value_type>(begin(), end());
    }

    /// @brief Compares elements of the two vectors.
    bool operator==(const ivec& other) const noexcept {
        return std::equal(begin(), end(), other.begin(), other.end());
    }

    /// @brief Returns the first element.
    reference front() noexcept {
        return data_[0];
    }

    /// @brief Returns the last element.
    reference back() noexcept {
        return data_[size_ - 1];
    }

    /// @brief Returns the first element, const.
    const_reference front() const noexcept {
        return data_[0];
    }

    /// @brief Returns the last element, const.
    const_reference back() const noexcept {
        return data_[size_ - 1];
    }

    void pop_back() noexcept {
        inr_assert(size_ > 0, "Size must not be 0.");
        if constexpr(!std::is_trivially_destructible_v<value_type>)
            data_[size_ - 1].~value_type();
        size_--;
    }

    const_iterator find(const_reference val) const noexcept {
        const_iterator it = begin();
        for(; it != end(); ++it) {
            if(*it == val) return it;
        }
        return it;
    }

    iterator find(const_reference val) noexcept {
        iterator it = begin();
        for(; it != end(); ++it) {
            if(*it == val) return it;
        }
        return it;
    }

    bool bfind(const T& val) const noexcept {
        for(const_pointer i = data_; i != data_ + size_; ++i) {
            if(*i == val) return true;
        }
        return false;
    }
};

} // namespace inr

#endif // INERTIA_ADT_IVECTOR_H
