// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_STLSTREAM_H
#define INERTIA_SUPPORT_STLSTREAM_H

/// @file Support/STLStream.h
/// @brief Allows to pass in an STL stream into Inertia's streams.

#include <inr/Support/Stream.h>

#include <ostream>

namespace inr {

/// @brief Allows passing in std::ostream into functions taking inr::stream.
class stlstream : public stream {
    std::ostream& os_;

    void writeImpl(cbuff_t ptr, size_type size) override {
        os_.write(ptr, std::streamsize(size));
    }

    void flushImpl() override {
        os_.flush();
    }

public:
    stlstream(std::ostream& os, size_type bufferSize = 0) :
        stream(bufferSize), os_(os) {}

    ~stlstream() override {
        setUnbuffered();
    }
};

} // namespace inr

#endif // INERTIA_SUPPORT_STLSTREAM_H
