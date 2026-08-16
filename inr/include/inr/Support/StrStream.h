// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_STRSTREAM_H
#define INERTIA_SUPPORT_STRSTREAM_H

/// @file Support/StrStream.h
/// @brief Provides a string stream class.

#include <inr/Support/Stream.h>

namespace inr {

/// @brief Stream into an std::string.
class sstream : public stream {
    std::string str_;

    void writeImpl(cbuff_t ptr, size_type size) override {
        str_.append(ptr, size);
    }

public:
    /// @brief Creates a new string stream.
    sstream(size_type bufferSize = 0) : stream(bufferSize) {}

    /// @brief Copies/Moves the caller's string into this stream.
    sstream(std::string str, size_type bufferSize = 0) :
        stream(bufferSize), str_(std::move(str)) {}

    const std::string& access() const {
        return str_;
    }

    /// @brief If buffering is present this can be unreliable.
    std::string& access() {
        return str_;
    }

    /// @brief Moves the string to the caller.
    std::string str() {
        return std::move(str_);
    }

    ~sstream() override {
        setUnbuffered();
    }
};

} // namespace inr

#endif // INERTIA_SUPPORT_STRSTREAM_H
