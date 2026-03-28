// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_MEMORYFILE_H
#define INERTIA_SUPPORT_MEMORYFILE_H

/// @file Support/MemoryFile.h
/// @brief Provides a way to open a file in memory.

#include <inr/ADT/StrView.h>
#include <inr/Support/CFile.h>

#include <cstdio>

namespace inr {

/// @brief Provides an abstraction for opening a file into memory.
class MemoryFile {
public:
    using value_type = char;
    using iterator = const value_type*;
    using const_iterator = iterator;
    using size_type = size_t;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    char* start_ = nullptr;
    char* end_ = nullptr;

public:
    MemoryFile() noexcept = default;
    /// @brief Reads the file into memory, does not reset read pointer.
    /// @param f File to read from, must be opened with "r".
    /// @param len How many bytes to read.
    /// @param insertNL whether it should insert a new line.
    MemoryFile(FILE* f, long len, bool insertNL = false) {
        openFromFILE(f, len, insertNL);
    }

    MemoryFile(const MemoryFile&) = delete;
    MemoryFile& operator=(const MemoryFile&) = delete;

    MemoryFile(MemoryFile&& other) noexcept :
        start_(other.start_), end_(other.end_) {
        other.start_ = other.end_ = nullptr;
    }

    MemoryFile& operator=(MemoryFile&& other) noexcept {
        if(this != &other) {
            start_ = other.start_;
            end_ = other.end_;

            other.start_ = other.end_ = nullptr;
        }
        return *this;
    }

    /// @brief Same as the one with FILE*.
    MemoryFile(CFile& f, long len, bool insertNL = false) :
        MemoryFile(f.getFile(), len, insertNL) {}

    /// @see `MemoryFile(FILE*, long, bool)` for more info.
    void openFromFILE(FILE* f, long len, bool insertNL = false) {
        clear();
        if(insertNL) len++;

        start_ = new char[len];
        fread(start_, 1, len, f);

        if(insertNL) {
            start_[len - 1] = '\n';
        }

        end_ = start_ + len;
    }

    /// @brief Frees the memory.
    void clear() noexcept {
        delete[] start_; // Passing in nullptr to delete is fine.
        start_ = end_ = nullptr;
    };

    /// @brief Returns whether its allocated or not.
    /// @return True if not allocated, false if is allocated.
    bool empty() const noexcept {
        return start_ == nullptr;
    }

    /// @brief Returns the memory as a string view.
    /// @return String view.
    sview view() const noexcept {
        return sview(start_, end_ - start_);
    }

    const char* data() const noexcept {
        return start_;
    }

    iterator begin() const noexcept {
        return data();
    }

    iterator end() const noexcept {
        return end_;
    }

    ~MemoryFile() noexcept {
        clear();
    }
};

} // namespace inr

#endif // INERTIA_SUPPORT_MEMORYFILE_H
