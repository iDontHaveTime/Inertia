// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_CFILE_H
#define INERTIA_SUPPORT_CFILE_H

/// @file Support/CFile.h
/// @brief Provides a wrapper for cstdio's FILE.

#include <cstdio>

namespace inr {

/// @brief Wraps cstdio's FILE pointer.
class CFile {
    FILE* f_;
    bool close_;

public:
    CFile() = delete;

    CFile(const CFile&) = delete;
    CFile& operator=(const CFile&) = delete;

    operator bool() const noexcept {
        return f_ != nullptr;
    }

    /// @brief Move constructor.
    CFile(CFile&& other) noexcept : f_(other.f_), close_(other.close_) {
        other.f_ = nullptr;
        other.close_ = false;
    }

    /// @brief Move operator.
    CFile& operator=(CFile&& other) noexcept {
        if(this != &other) {
            f_ = other.f_;
            close_ = other.close_;

            other.f_ = nullptr;
            other.close_ = false;
        }

        return *this;
    }

    /// @brief Grabs the file pointer given.
    /// @param f File stream.
    /// @param takeOwnership Should the file be closed by the class.
    CFile(FILE* f, bool takeOwnership) noexcept :
        f_(f), close_(takeOwnership) {}

    /// @brief A wrapper for fopen.
    /// @param fname File name.
    /// @param mode Opening mode.
    /// @param takeOwnership Should this wrapper close the file.
    CFile(const char* fname, const char* mode,
          bool takeOwnership = true) noexcept :
        CFile(fopen(fname, mode), takeOwnership) {}

    /// @brief Closes the file stream.
    /// @note Only works if took ownership.
    void close() noexcept {
        if(f_ && close_) {
            fclose(f_);
            f_ = nullptr;
            close_ = false;
        }
    }

    /// @brief Gets the FILE pointer stored in the class.
    /// @return FILE pointer.
    FILE* getFile() const noexcept {
        return f_;
    }

    ~CFile() noexcept {
        close();
    }
};

} // namespace inr

#endif // INERTIA_SUPPORT_CFILE_H
