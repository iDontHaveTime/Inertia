#ifndef INERTIA_ADT_ARRVIEW_H
#define INERTIA_ADT_ARRVIEW_H

/// @file ADT/ArrView.h
/// @brief Provides an array wrapper class similar to llvm::ArrayRef<T>.

#include <iterator>
#include <vector>

namespace inr {

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

    iterator begin() const noexcept {
        return data_;
    }
    iterator end() const noexcept {
        return data_ + len_;
    }
    reverse_iterator rbegin() const noexcept {
        return std::reverse_iterator<iterator>(data_ + len_);
    }
    reverse_iterator rend() const noexcept {
        return std::reverse_iterator<iterator>(data_);
    }

    /// @brief Checks whether the array is empty or not.
    /// @return True if empty, false if not.
    bool empty() const noexcept {
        return !len_;
    }

    const_pointer data() const noexcept {
        return data_;
    }

    size_type size() const noexcept {
        return len_;
    }

    const_reference front() const noexcept {
        return *data_;
    }

    const_reference back() const noexcept {
        return *(data_ + (len_ - 1));
    }

    const_reference operator[](difference_type index) const noexcept {
        return data_[index];
    }

    std::vector<T> vec() const {
        return std::vector<T>(begin(), end());
    }
};

} // namespace inr

#endif // INERTIA_ADT_ARRVIEW_H
