// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_ADT_ARRVIEW_H
#define INERTIA_ADT_ARRVIEW_H

/// @file ADT/ArrView.h
/// @brief Provides an array wrapper class similar to llvm::ArrayRef<T>.

#include <inr/ADT/IVector.h>

#include <iterator>
#include <vector>

namespace inr {

/// @brief A non-owning array type.
template<typename T>
class arrview {
public:
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = const_pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

private:
    const_pointer data_ = nullptr;
    size_type len_ = 0;

public:
    /// @brief Creates an empty array view.
    constexpr arrview() noexcept = default;

    constexpr arrview(const arrview&) noexcept = default;
    constexpr arrview& operator=(const arrview&) noexcept = default;

    constexpr arrview(arrview&&) noexcept = default;
    constexpr arrview& operator=(arrview&&) noexcept = default;

    /// @brief Creates a new array view from a vector.
    /// @param vec Vector to create it from.
    /// @note This does not update with the vector.
    arrview(const std::vector<T>& vec) noexcept :
        data_(vec.data()), len_(vec.size()) {}

    /// @brief Creates a new array view from an inline vector.
    template<size_t N>
    arrview(const ivec<T, N>& vec) noexcept :
        data_(vec.data()), len_(vec.size()) {}

    /// @brief Creates a new array view from pointer and length.
    /// @param data Pointer to the start of the array.
    /// @param length How many elements in the array.
    constexpr arrview(const_pointer data, size_type length) noexcept :
        data_(data), len_(length) {}

    /// @brief Creates a new array view from start and end pointers.
    /// @param ptr1 Pointer to the start of the array.
    /// @param ptr2 Pointer to the end of the array.
    constexpr arrview(const_pointer ptr1, const_pointer ptr2) noexcept :
        data_(ptr1), len_(ptr2 > ptr1 ? ptr2 - ptr1 : 0) {}

    /// @brief Creates a new array view from T[] arrays.
    /// @param arr Array.
    template<typename AT, size_t N>
    constexpr arrview(AT (&arr)[N]) noexcept : data_(arr), len_(N) {}

    /// @brief Creates a new array view from a span.
    /// @param data The base of the span.
    /// @param start Where the span starts from.
    /// @param n How many elements.
    ///
    /// For example if we have:
    /// ```c++
    /// int arr[] = {1, 2, 3, 4, 5};
    /// ```
    /// Then an array view of `arrview<int>(arr, 1, 2)` would contain `{2, 3}`.
    constexpr arrview(const_pointer data, size_type start, size_type n) :
        data_(data + start), len_(n) {}

    constexpr const_iterator begin() const noexcept {
        return data_;
    }
    constexpr const_iterator end() const noexcept {
        return data_ + len_;
    }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(data_ + len_);
    }
    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(data_);
    }

    /// @brief Checks whether the array is empty or not.
    /// @return True if empty, false if not.
    constexpr bool empty() const noexcept {
        return !len_;
    }

    /// @brief Returns the pointer to the first element.
    constexpr const_pointer data() const noexcept {
        return data_;
    }

    /// @brief Returns the size of the array.
    /// @return How many elements are in the array.
    constexpr size_type size() const noexcept {
        return len_;
    }

    /// @brief Returns a reference to the first element.
    /// @return Const reference to the first element.
    constexpr const_reference front() const noexcept {
        return *data_;
    }

    /// @brief Returns a reference to the last element.
    /// @return Const reference to the last element.
    constexpr const_reference back() const noexcept {
        return *(data_ + (len_ - 1));
    }

    /// @brief Accesses the array without bounds checking.
    /// @return Const reference to the element.
    constexpr const_reference operator[](difference_type index) const noexcept {
        return data_[index];
    }

    /// @brief Creates a vector from the array.
    /// @return Vector with the elements copied from the array.
    std::vector<T> vec() const {
        return std::vector<T>(begin(), end());
    }

    constexpr bool operator==(arrview<T> other) const noexcept {
        return std::equal(data_, data_ + len_, other.data_,
                          other.data_ + other.len_);
    }
};

/// @brief A non-owning mutable array type.
template<typename T>
class ncarrview {
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
    pointer data_ = nullptr;
    size_type len_ = 0;

public:
    /// @brief Creates an empty array view.
    constexpr ncarrview() noexcept = default;

    constexpr ncarrview(const ncarrview&) noexcept = default;
    constexpr ncarrview& operator=(const ncarrview&) noexcept = default;

    constexpr ncarrview(ncarrview&&) noexcept = default;
    constexpr ncarrview& operator=(ncarrview&&) noexcept = default;

    /// @see arrview<T> implementation.
    ncarrview(std::vector<T>& vec) noexcept :
        data_(vec.data()), len_(vec.size()) {}

    /// @see arrview<T> implementation.
    template<size_t N>
    ncarrview(ivec<T, N>& vec) noexcept : data_(vec.data()), len_(vec.size()) {}

    /// @see arrview<T> implementation.
    constexpr ncarrview(pointer data, size_type length) noexcept :
        data_(data), len_(length) {}

    /// @see arrview<T> implementation.
    constexpr ncarrview(pointer ptr1, pointer ptr2) noexcept :
        data_(ptr1), len_(ptr2 > ptr1 ? ptr2 - ptr1 : 0) {}

    /// @see arrview<T> implementation.
    template<typename AT, size_t N>
    constexpr ncarrview(AT (&arr)[N]) noexcept : data_(arr), len_(N) {}

    /// @see arrview<T> implementation.
    constexpr ncarrview(pointer data, size_type start, size_type n) :
        data_(data + start), len_(n) {}

    constexpr iterator begin() noexcept {
        return data_;
    }
    constexpr iterator end() noexcept {
        return data_ + len_;
    }

    constexpr const_iterator begin() const noexcept {
        return data_;
    }
    constexpr const_iterator end() const noexcept {
        return data_ + len_;
    }

    constexpr reverse_iterator rbegin() noexcept {
        return reverse_iterator(data_ + len_);
    }
    constexpr reverse_iterator rend() noexcept {
        return reverse_iterator(data_);
    }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(data_ + len_);
    }
    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(data_);
    }

    /// @brief Checks whether the array is empty or not.
    /// @return True if empty, false if not.
    constexpr bool empty() const noexcept {
        return !len_;
    }

    /// @brief Returns the pointer to the first element, const.
    constexpr const_pointer data() const noexcept {
        return data_;
    }

    /// @brief Returns the pointer to the first element.
    constexpr pointer data() noexcept {
        return data_;
    }

    /// @brief Returns the size of the array.
    /// @return How many elements are in the array.
    constexpr size_type size() const noexcept {
        return len_;
    }

    /// @brief Returns a const reference to the first element.
    /// @return Const reference to the first element.
    constexpr const_reference front() const noexcept {
        return *data_;
    }

    /// @brief Returns a const reference to the last element.
    /// @return Const reference to the last element.
    constexpr const_reference back() const noexcept {
        return *(data_ + (len_ - 1));
    }

    /// @brief Returns a reference to the first element.
    /// @return Reference to the first element.
    constexpr reference front() noexcept {
        return *data_;
    }

    /// @brief Returns a reference to the last element.
    /// @return Reference to the last element.
    constexpr reference back() noexcept {
        return *(data_ + (len_ - 1));
    }

    /// @brief Accesses the array without bounds checking.
    /// @return Const reference to the element.
    constexpr const_reference operator[](difference_type index) const noexcept {
        return data_[index];
    }

    /// @brief Accesses the array without bounds checking.
    /// @return Reference to the element.
    constexpr reference operator[](difference_type index) noexcept {
        return data_[index];
    }

    /// @brief Creates a vector from the array.
    /// @return Vector with the elements copied from the array.
    std::vector<T> vec() const {
        return std::vector<T>(begin(), end());
    }

    constexpr operator arrview<T>() const noexcept {
        return arrview<T>(data_, len_);
    }

    constexpr bool operator==(ncarrview<T> other) const noexcept {
        return std::equal(data_, data_ + len_, other.data_,
                          other.data_ + other.len_);
    }
};

} // namespace inr

#endif // INERTIA_ADT_ARRVIEW_H
