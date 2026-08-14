// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_ADT_ARRVIEW_H
#define INERTIA_ADT_ARRVIEW_H

/// @file ADT/ArrView.h
/// @brief Provides a class similar to std::span.

#include <inr/ADT/IVector.h>
#include <inr/Support/Assert.h>

#include <initializer_list>
#include <iterator>

namespace inr {

/// @brief Provides a view over an array, similar to std::span.
/// @note Does not allow modifying the elements.
template<typename T>
class arrview {
public:
    using value_type = T;
    using const_pointer = const value_type*;
    using const_reference = const value_type&;
    using const_iterator = const_pointer;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

private:
    const_pointer data_{};
    size_type size_{};

public:
    constexpr arrview() = default;

    constexpr arrview(const arrview&) = default;
    constexpr arrview& operator=(const arrview&) = default;

    constexpr arrview(arrview&&) noexcept = default;
    constexpr arrview& operator=(arrview&&) noexcept = default;

    ~arrview() = default;

    /// @brief Constructs a view from an inline vector.
    /// @note Modifying the vector will invalidate this.
    template<size_type N>
    arrview(const ivec<value_type, N>& vec) :
        data_(vec.data()), size_(vec.size()) {}

    arrview(const std::vector<value_type>& vec) :
        data_(vec.data()), size_(vec.size()) {}

    template<size_type N>
    constexpr arrview(const value_type (&arr)[N]) : data_(arr), size_(N) {}

    constexpr arrview(const_pointer data, size_type n) :
        data_(data), size_(n) {}

    constexpr arrview(std::initializer_list<value_type> l) :
        data_(l.begin()), size_(l.size()) {}

    const_pointer data() const {
        return data_;
    }

    const_reference operator[](size_type n) const {
        inr_assert(n < size_, "arrview operator[]: out of bounds");
        return data_[n];
    }

    const_reference at(size_type n) const {
        inr_assert(n < size_, "arrview at(): out of bounds");
        return data_[n];
    }

    size_type size() const {
        return size_;
    }

    size_type capacity() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
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

    bool operator==(const arrview& other) const {
        return std::equal(begin(), end(), other.begin(), other.end());
    }

    const_reference front() const {
        inr_assert(size_ != 0, "arrview front(): size was zero");
        return data_[0];
    }

    const_reference back() const {
        inr_assert(size_ != 0, "arrview back(): size was zero");
        return data_[size_ - 1];
    }

    const_iterator find(const_reference val) const {
        return std::find(begin(), end(), val);
    }

    bool find_bool(const_reference val) const {
        return find(val) != end();
    }
};

} // namespace inr

#endif // INERTIA_ADT_ARRVIEW_H
