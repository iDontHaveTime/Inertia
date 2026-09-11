// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_ADT_STORAGE_H
#define INERTIA_ADT_STORAGE_H

/// @file ADT/Storage.h
/// @brief Provides a class similar to using a vector with unique ptr.

#include <memory>
#include <utility>
#include <vector>

namespace inr {

/// @brief Provides a stable pointer storage that automatically frees.
/// @todo Use a custom implementation.
/// @note Do NOT free pointer returned by this.
template<typename T>
class storage {
    std::vector<std::unique_ptr<T>> storage_;

public:
    storage() = default;

    storage(const storage& other) = default;
    storage& operator=(const storage&) = default;

    storage(storage&&) noexcept = default;
    storage& operator=(storage&&) noexcept = default;

    ~storage() = default;

    /// @brief Creates a new element and returns a pointer to it.
    template<typename... Args>
    T* create(Args&&... args) {
        return storage_.emplace_back(new T(std::forward<Args>(args)...)).get();
    }

    /// @brief Move an existing unique_ptr into this storage.
    T* move(std::unique_ptr<T>&& elem) {
        return storage_.emplace_back(std::move(elem)).get();
    }

    /// @brief Move this element into this storage and assume ownership.
    T* own(T* elem) {
        return storage_.emplace_back(elem).get();
    }
};

} // namespace inr

#endif // INERTIA_ADT_STORAGE_H
