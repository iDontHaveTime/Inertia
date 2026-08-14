// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_CSTREAM_H
#define INERTIA_SUPPORT_CSTREAM_H

/// @file Support/CStream.h
/// @brief Provides a stream class for stdio's FILE.

#include <inr/Support/Assert.h>
#include <inr/Support/Stream.h>

#include <cstdio>

namespace inr {

/// @brief Provides a stream interface for stdio's FILE.
class cstream : public stream {
    FILE* handle_;
    bool close_;

    void writeImpl(cbuff_t ptr, size_type size) override {
        inr_assert(handle_ != nullptr, "File handle is nullptr");
        std::fwrite(ptr, 1, size, handle_);
    }

    void flushImpl() override {
        if(handle_) std::fflush(handle_);
    }

public:
    /// @brief Creates a new stdio FILE stream.
    /// @param handle The FILE to stream to.
    /// @param shouldClose Should this stream assume ownership.
    /// @param bufferSize Size of the buffer that the stream will create.
    /// @note The buffer size default is zero because most stdio streams already
    /// have one.
    cstream(FILE* handle, bool shouldClose, size_type bufferSize = 0) :
        stream(bufferSize), handle_(handle), close_(shouldClose) {}

    ~cstream() override {
        setUnbuffered();
        if(close_ && handle_) std::fclose(handle_);
    }
};

} // namespace inr

#endif // INERTIA_SUPPORT_CSTREAM_H
