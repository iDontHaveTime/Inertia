// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_STREAM_H
#define INERTIA_SUPPORT_STREAM_H

/// @file Support/Stream.h
/// @brief Provides a stream class as a replacement for STL's streams.

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

namespace inr {

enum class ColorOverride : uint8_t {
    AUTO,
    ALWAYS,
    NEVER,
};
/// @brief Overrides whether or not colors are available or not.
extern ColorOverride global_color_override;

/// @brief Terminal colors.
enum class col {
    BLACK = 30,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    BRIGHT_BLACK = 90,
    BRIGHT_RED,
    BRIGHT_GREEN,
    BRIGHT_YELLOW,
    BRIGHT_BLUE,
    BRIGHT_MAGENTA,
    BRIGHT_CYAN,
    BRIGHT_WHITE,
};

/// @brief Base class for all output streams.
///
/// Somewhat follows STL stream semantics with the operator<<.
class stream {
protected:
    /// @brief One buffer character.
    /// @note Should always be char.
    using buffchar_t = char;
    /// @brief Pointer to the buffer character.
    using buff_t = buffchar_t*;
    /// @brief Pointer to a constant buffer character.
    using cbuff_t = const buffchar_t*;
    /// @brief Alias for std::size_t.
    using size_type = std::size_t;

private:
    buff_t start_, cur_, end_;
    bool reversedColor_;

protected:
    /// @brief Implementation for the write method for the derived stream.
    /// @param ptr Pointer to the data.
    /// @param size Size of the data.
    virtual void writeImpl(cbuff_t ptr, size_type size) = 0;
    /// @brief A hook for when flushing the buffer.
    ///
    /// This is usually empty, although for example when using stdio's FILE
    /// handle you should call fflush.
    virtual void flushImpl() {};

public:
    /// @brief What is the default buffer size if it wasn't specified.
    constexpr static size_type DEFAULT_BUFFER_SIZE = 0x2000;

    /// @brief Creates a new stream with the specified buffer size.
    /// @param bufferSize Size of the buffer to create, 0 for none.
    stream(size_type bufferSize = DEFAULT_BUFFER_SIZE);

    /// @brief Streams shouldn't be copyable.
    stream(const stream&) = delete;
    stream& operator=(const stream&) = delete;

    /// @brief Why would you ever construct a stream from moving.
    stream(stream&&) = delete;
    /// @brief Why would you ever move a stream.
    stream& operator=(stream&&) = delete;

    /// @brief Returns the size of the buffer.
    size_type getBufferSize() const {
        return end_ - start_;
    }

    /// @brief Writes to the stream.
    /// @param data The data to write.
    /// @param size Size of the data.
    /// @return *this.
    stream& write(cbuff_t data, size_type size);

    /// @brief May change the stream's buffer.
    /// @param size Size for the new buffer.
    void setBufferSize(size_type size);

    /// @brief Removes this stream's buffer.
    void setUnbuffered() {
        setBufferSize(0);
    }

    /// @brief Returns how many chars are currently in the buffer.
    size_type getCharsInBuffer() const {
        return end_ - cur_;
    }

    /// @brief Flushes the stream.
    void flush() {
        if(cur_ != start_) {
            writeImpl(start_, getCharsInBuffer());
            cur_ = start_;
        }
        flushImpl();
    }

    /// @brief Returns whether or not the stream is on a terminal.
    virtual bool isDisplayed() const {
        return false;
    }

    /// @brief Returns whether or not the stream supports colors.
    virtual bool hasColors() const {
        return global_color_override == ColorOverride::ALWAYS;
    }

    /// @brief Writes the specified amount of spaces.
    /// @param space How many spaces should be written.
    ///
    /// Internally uses a decently sized space buffer so that each write is many
    /// spaces at once instead of one, so prefer this over writing spaces one by
    /// one.
    stream& indent(unsigned space);

    /// @brief Writes "null" to the stream.
    stream& operator<<(std::nullptr_t) {
        return write("null", 4);
    }

    /// @brief Writes a character to the stream.
    stream& operator<<(char c) {
        return write(&c, 1);
    }

    /// @brief Writes a signed character to the stream.
    stream& operator<<(signed char c) {
        return write((cbuff_t)&c, 1);
    }

    /// @brief Writes an unsigned character to the stream.
    stream& operator<<(unsigned char c) {
        return write((cbuff_t)&c, 1);
    }

    /// @brief Writes a boolean to the stream.
    stream& operator<<(bool b) {
        return write((b ? "true" : "false"), (b ? 4 : 5));
    }

    stream& operator<<(const void* ptr) {
        uintptr_t ptrVal = uintptr_t(ptr);
        buffchar_t buff[(sizeof(void*) * 2) + 6] = {'0', 'x'};
        auto result = std::to_chars(buff + 2, buff + sizeof(buff), ptrVal, 16);

        if(result.ec == std::errc()) {
            write(buff, result.ptr - buff);
        }

        return *this;
    }

    /// @brief Writes either an integral or a floating point value to the
    /// stream.
    template<typename T>
    requires std::integral<T> || std::floating_point<T>
    stream& operator<<(T n) {
        buffchar_t buff[(std::is_floating_point_v<T>
                             ? std::numeric_limits<T>::max_digits10
                             : std::numeric_limits<T>::digits10) +
                        0x20];

        auto result = std::to_chars(buff, buff + sizeof(buff), n);
        if(result.ec == std::errc()) {
            write(buff, result.ptr - buff);
        }

        return *this;
    }

    /// @brief Writes a null terminated string to the stream.
    stream& operator<<(const char* cstr) {
        return write(cstr, std::strlen(cstr));
    }

    /// @brief Writes a string to the stream.
    stream& operator<<(const std::string& str) {
        return write(str.data(), str.size());
    }

    /// @brief Writes a string view to the stream.
    stream& operator<<(std::string_view sv) {
        return write(sv.data(), sv.size());
    }

    /// @brief Does not flush or free the buffer because it may cause a pure
    /// virtual call.
    virtual ~stream() = default;

    /// @brief Resets colors to the default ones.
    stream& resetColor() {
        if(hasColors()) {
            write("\033[0m", 4);
            reversedColor_ = false;
        }
        return *this;
    }

    /// @brief Reverses the colors of the background and foreground.
    stream& reverseColor() {
        if(hasColors()) {
            if(reversedColor_) {
                write("\033[27m", 5);
            }
            else write("\033[7m", 4);
            reversedColor_ = !reversedColor_;
        }
        return *this;
    }

    /// @brief Changes color (maybe boldness too) of the stream.
    stream& changeColor(col color, bool bold = false, bool bg = false) {
        if(hasColors()) {
            int code = int(color);
            if(bg) code += 10;

            write("\033[", 2);
            if(bold) {
                write("1;", 2);
            }

            *this << code << 'm';
        }
        return *this;
    }

    /// @brief Sets the boldness of the stream.
    stream& setBold(bool bold = true) {
        if(hasColors()) {
            if(bold) {
                write("\033[1m", 4);
            }
            else {
                write("\033[22m", 5);
            }
        }
        return *this;
    }

    /// @brief Resets foreground color.
    stream& removeFColor() {
        if(hasColors()) write("\033[39m", 5);
        return *this;
    }

    /// @brief Resets background color.
    stream& removeBColor() {
        if(hasColors()) write("\033[49m", 5);
        return *this;
    }

    /// @brief Operator overload for `changeColor(...)`.
    stream& operator<<(col color) {
        return changeColor(color);
    }
};

/// @brief Buffered stdout stream.
extern stream& out();
/// @brief Unbuffered stderr stream.
extern stream& err();
/// @brief Buffered stderr stream.
extern stream& log();

} // namespace inr

#endif // INERTIA_SUPPORT_STREAM_H
