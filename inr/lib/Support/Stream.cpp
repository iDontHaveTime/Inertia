// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Support/Stream.h>
#include <inr/Support/Version.h>

#ifndef __unix__
#define INR_USE_STANDARD_STREAM
#endif

#ifdef INR_USE_STANDARD_STREAM
#include <inr/Support/CStream.h>
#endif

namespace inr {

/// @brief By default should be auto.
ColorOverride global_color_override = ColorOverride::AUTO;

stream::stream(size_type bufferSize) :
    start_(nullptr), cur_(nullptr), end_(nullptr), reversedColor_(false) {
    setBufferSize(bufferSize);
}

stream& stream::write(cbuff_t data, size_type size) {
    if(!start_) writeImpl(data, size);
    else {
        size_type bufsiz = getBufferSize();

        while(size) {
            size_type spaceLeft = getCharsInBuffer();

            if(spaceLeft >= bufsiz) {
                flush();
                writeImpl(data, size);
                break;
            }

            if(!spaceLeft) {
                flush();
                spaceLeft = bufsiz;
            }

            size_type toCopy = std::min(size, spaceLeft);

            std::memcpy(cur_, data, toCopy);

            cur_ += toCopy;
            data += toCopy;
            size -= toCopy;
        }
    }
    return *this;
}

void stream::setBufferSize(size_type size) {
    flush();
    if(!size) {
        delete[] start_;
        start_ = cur_ = end_ = nullptr;
        return;
    }

    if(size != getBufferSize()) {
        delete[] start_;
        start_ = cur_ = new buffchar_t[size];
        end_ = start_ + size;
    }
}

constexpr unsigned SPACE_BUFFER_SIZE = 64;

stream& stream::indent(unsigned space) {
    class SpaceBuffer {
        char spaces_[SPACE_BUFFER_SIZE];

    public:
        constexpr SpaceBuffer() : spaces_() {
            for(unsigned i = 0; i < SPACE_BUFFER_SIZE; i++) {
                spaces_[i] = ' ';
            }
        }
        constexpr ~SpaceBuffer() = default;

        constexpr const char* getSpaces() const {
            return spaces_;
        }
    };
    constexpr static SpaceBuffer space_buffer{};

    while(space) {
        unsigned to_write = std::min(space, SPACE_BUFFER_SIZE);
        write(space_buffer.getSpaces(), to_write);

        space -= to_write;
    }
    return *this;
}

#ifdef INR_USE_STANDARD_STREAM
stream& out() {
    static cstream stdout_s(stdout, false);
    return stdout_s;
}

stream& err() {
    static cstream stderr_s(stderr, false);
    return stderr_s;
}

stream& log() {
    static cstream log_s(stderr, false, stream::DEFAULT_BUFFER_SIZE);
    return log_s;
}
#endif

stream& operator<<(stream& os, Version ver) {
    return os << ver.getMajor() << '.' << ver.getMinor() << '.'
              << ver.getPatch();
}

} // namespace inr
