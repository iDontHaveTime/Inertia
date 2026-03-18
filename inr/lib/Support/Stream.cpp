// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/Support/Stream.h>
#include <unistd.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>

namespace inr {

ColorOverride global_color_override = ColorOverride::AUTO;

raw_stream::raw_stream(size_t bufferSize) {
    /// The stream should start unbuffered.
    buffStart = buffCur = buffEnd = nullptr;
    buffStatus = BufferedStatus::UNBUFFERED;

    /// If buffer allocation is needed.
    if(bufferSize != 0) {
        setBufferSize(bufferSize);
    }
}

void raw_stream::setBufferSize(size_t size) {
    /// Free the buffer if needed.
    setUnbuffered();
    /// If the size is zero return since we already freed the buffer.
    if(size == 0) return;
    /// Allocate a new buffer.
    buffStart = buffCur = new char[size];
    buffEnd = buffStart + size;
    buffStatus = BufferedStatus::INTERNAL;
}

void raw_stream::setUnbuffered() {
    /// If a buffer already exists, free it.
    if(buffStatus == BufferedStatus::INTERNAL && buffStart) {
        flush();
        delete[] buffStart;
    }
    /// Set the status to unbuffered and reset pointers.
    buffStatus = BufferedStatus::UNBUFFERED;
    buffStart = buffCur = buffEnd = nullptr;
}

/// @brief How many spaces does the space buffer hold.
/// Basically how many spaces at once can indentation write.
constexpr unsigned SPACE_BUFFER_SIZE = 16;

raw_stream& raw_stream::indent(unsigned space) {
    /// Allocate a static buffer, no need to readjust stack every time.
    static const char space_buffer[SPACE_BUFFER_SIZE] = {
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
    while(space) {
        /// Write the number of spaces left.
        unsigned to_write = std::min(space, SPACE_BUFFER_SIZE);
        write(space_buffer, to_write);

        space -= to_write;
    }
    return *this;
}

raw_stream& raw_stream::write(const char* ptr, size_t size) {
    /// If unbuffered simply just write to the stream.
    if(buffStatus == BufferedStatus::UNBUFFERED) writeImpl(ptr, size);
    else {
        /// If buffered we loop until no more bytes left to write.
        size_t bytesLeft = size;

        while(bytesLeft) {
            size_t spaceLeft = buffEnd - buffCur;

            /// If the write is more than the buffer just write all at once.
            if(bytesLeft >= getBufferSize()) {
                flush();
                writeImpl(ptr, bytesLeft);
                break;
            }

            if(!spaceLeft) {
                flush();
                spaceLeft = getBufferSize();
            }

            size_t toCopy = std::min(bytesLeft, spaceLeft);

            memcpy(buffCur, ptr, toCopy);

            buffCur += toCopy;
            ptr += toCopy;
            bytesLeft -= toCopy;
        }
    }
    return *this;
}

struct CheckTerminalColors {
    bool color;

    CheckTerminalColors() {
        const char* term = getenv("TERM");
        if(getenv("NO_COLOR")) {
            color = false;
            return;
        }

        if(!term || strcmp(term, "dumb") == 0) {
            color = false;
            return;
        }

        color = true;
    }
};

stream::stream(int fd, bool shouldClose, size_t bufferSize) :
    raw_stream(bufferSize), fd_(fd), close_(shouldClose) {
    /// stdout and stderr can have colors if are in a TTY.
    if(fd == 1 || fd == 2) {
        if(bool(isatty(fd))) {
            displayed_ = true;
            static CheckTerminalColors colorsPresent;
            colors_ = colorsPresent.color;
        }
    }
    else {
        displayed_ = false;
        colors_ = false;
    }
}

stream::~stream() noexcept {
    /// Remove the buffer and close the fd if prompted.
    setUnbuffered();
    if(close_) close(fd_);
}

void stream::writeImpl(const char* ptr, size_t size) {
    /// @todo Error handling.
    ::write(fd_, ptr, size);
}

raw_stream& outs() {
    /// Lazy init stdout.
    static stream stdout_stream(1, false);
    return stdout_stream;
}

raw_stream& errs() {
    /// Lazy init stderr.
    static stream stderr_stream(2, false, 0);
    return stderr_stream;
}

void standard_file_stream::writeImpl(const char* ptr, size_t size) {
    if(file_) fwrite(ptr, 1, size, file_);
}

standard_file_stream::~standard_file_stream() noexcept {
    setUnbuffered();
    if(file_ && close_) fclose(file_);
}

} // namespace inr