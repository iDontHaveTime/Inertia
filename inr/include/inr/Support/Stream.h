// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_STREAM_H
#define INERTIA_SUPPORT_STREAM_H

/// @file Support/Stream.h
/// @brief Contains classes relating to streams.
///
/// Only contains output streams, no input streams.

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

namespace inr {

enum class ColorOverride : uint8_t { AUTO, ALWAYS, NEVER };
extern ColorOverride global_color_override;

enum class Colors {
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
    BRIGHT_WHITE
};

using col = Colors;

class raw_stream;

using stream_manipulator = raw_stream& (*)(raw_stream&);

/// @brief An abstract output stream class.
class raw_stream {
protected:
    /// @brief Describes the status of the stream's buffer.
    enum class BufferedStatus : uint8_t { UNBUFFERED, INTERNAL };
    /// @brief Pointer to the data type of the buffer.
    char *buffStart, *buffCur, *buffEnd;
    /// @brief Status of the stream's buffering.
    BufferedStatus buffStatus;
    bool reversedColor = false;

    /// @brief Implementation of the write, class-dependent.
    /// @param ptr Pointer to the data.
    /// @param size Size of the data.
    virtual void writeImpl(const char* ptr, size_t size) = 0;

    /// @brief The buffer size constructors default to.
    constexpr static size_t DEFAULT_BUFFER_SIZE = 0x2000;

public:
    /// @brief Constructs a stream with the selected buffer size.
    /// @param bufferSize Size for the buffer, 0 for unbuffered.
    raw_stream(size_t bufferSize = DEFAULT_BUFFER_SIZE);

    /// @brief Returns the buffer size of the stream.
    /// @return Buffer size.
    size_t getBufferSize() const noexcept {
        return buffEnd - buffStart;
    }

    /// @brief Writes data to the stream.
    /// @param ptr Pointer to the data.
    /// @param size Size of the data.
    /// @return *this
    raw_stream& write(const char* ptr, size_t size);

    /// @brief Allocates a new buffer for the stream.
    /// @param size The size for the new buffer.
    void setBufferSize(size_t size = DEFAULT_BUFFER_SIZE);
    /// @brief Sets the stream to be unbuffered.
    void setUnbuffered();
    /// @brief Flushes the buffer.
    void flush() {
        // nullptr if unbuffered so no need to check.
        if(buffCur != buffStart) {
            writeImpl(buffStart, buffCur - buffStart);
            buffCur = buffStart;
        }
    }

    /// @brief Is the stream displayed on a tty or console window.
    /// @return True if displayed, false if not.
    virtual bool isDisplayed() const {
        return false;
    }

    /// @brief Whether the stream supports colors or not.
    /// @return True if supports, false if not.
    virtual bool hasColors() const {
        return isDisplayed();
    }

    /// @brief Writes N amount of spaces to the stream.
    /// @param space Amount of spaces to write.
    /// @return *this
    ///
    /// Uses a temporary space buffer, can write up to 16 spaces per iteration.
    raw_stream& indent(unsigned space);

    /// @brief Writes 'null' to the stream.
    /// @return *this
    raw_stream& operator<<(std::nullptr_t) {
        return write("null", 4);
    }

    /// @brief Writes a character to the stream.
    /// @param c Character.
    /// @return *this
    raw_stream& operator<<(char c) {
        return write(&c, sizeof(c));
    }

    /// @brief Writes an unsigned character to the stream.
    /// @param c Character.
    /// @return *this
    raw_stream& operator<<(unsigned char c) {
        return *this << char(c);
    }

    /// @brief Writes a signed character to the stream.
    /// @param c Character.
    /// @return *this
    raw_stream& operator<<(signed char c) {
        return *this << char(c);
    }

    /// @brief Writes a boolean to the stream.
    /// @param c Boolean value.
    /// @return *this
    raw_stream& operator<<(bool b) {
        return write(b ? "true" : "false", b ? 4 : 5);
    }

    /// @brief Writes either an integral or a floating point value to the
    /// stream.
    /// @param n The value to write.
    /// @return *this
    template<typename T>
    requires std::integral<T> || std::floating_point<T>
    raw_stream& operator<<(T n) {
        char buff[(std::is_floating_point_v<T>
                       ? std::numeric_limits<T>::max_digits10
                       : std::numeric_limits<T>::digits10) +
                  0x20];

        auto result = std::to_chars(buff, buff + sizeof(buff), n);
        if(result.ec == std::errc()) {
            write(buff, result.ptr - buff);
        }

        return *this;
    }

    /// @brief Writes a pointer's address to the stream.
    /// @param ptr Address to write.
    /// @return *this
    raw_stream& operator<<(const void* ptr) {
        char buff[(sizeof(void*) * 2) + 8];

        auto result =
            std::to_chars(buff, buff + sizeof(buff), (uintptr_t)ptr, 16);
        if(result.ec == std::errc()) {
            write(buff, result.ptr - buff);
        }

        return *this;
    }

    /// @brief Writes a C-string to the stream.
    /// @param cstr String.
    /// @return *this
    raw_stream& operator<<(const char* cstr) {
        return write(cstr, strlen(cstr));
    }

    /// @brief Writes an std::string to the stream.
    /// @param str String.
    /// @return *this
    raw_stream& operator<<(const std::string& str) {
        return write(str.data(), str.size());
    }

    /// @brief Writes an std::string_view to the stream.
    /// @param str String.
    /// @return *this
    raw_stream& operator<<(const std::string_view& str) {
        return write(str.data(), str.size());
    }

    /// @brief Default destructor.
    virtual ~raw_stream() noexcept {
        setUnbuffered();
    }

    /// @brief Resets the colors to the terminal's default.
    /// @return *this
    raw_stream& resetColor() {
        if(hasColors()) {
            write("\033[0m", 4);
            reversedColor = false;
        }
        return *this;
    }

    /// @brief Reverses the colors of the FG and BG.
    /// @return *this
    raw_stream& reverseColor() {
        if(hasColors()) {
            if(reversedColor) {
                *this << "\033[27m";
            }
            else *this << "\033[7m";
            reversedColor = !reversedColor;
        }
        return *this;
    }

    /// @brief Change the stream's colors.
    /// @param color Color to change to.
    /// @param bold Should the text be bold.
    /// @param bg Set the background, if false sets the text (foreground).
    /// @return *this
    raw_stream& changeColor(Colors color, bool bold = false, bool bg = false) {
        if(hasColors()) {
            int code = int(color);
            if(bg) {
                code += 10;
            }

            *this << "\033[";

            if(bold) *this << "1;";

            *this << code << "m";
        }
        return *this;
    }

    /// @brief Call stream manipulator on this stream.
    /// @param sm Stream manipulator.
    /// @return *this
    raw_stream& operator<<(stream_manipulator sm) {
        return sm(*this);
    }
};

/// @brief Output stream to a file.
class stream : public raw_stream {
protected:
    /// @brief Stream's file descriptor.
    int fd_;
    /// @brief Should the stream close the file descriptor.
    bool close_;
    /// @brief Is the stream displayed on a console or a tty window.
    bool displayed_;
    /// @brief Does the stream support colors.
    bool colors_;
    /// @brief Overriden write implementation to write to the fd.
    void writeImpl(const char* ptr, size_t size) override;

public:
    /// @brief Default stream constructor.
    /// @param fd File descriptor to use.
    /// @param shouldClose Should the stream close the file descriptor.
    /// @param bufferSize Size of the buffer the stream will use.
    stream(int fd, bool shouldClose,
           size_t bufferSize = raw_stream::DEFAULT_BUFFER_SIZE);

    bool isDisplayed() const override {
        return displayed_;
    }

    bool hasColors() const override {
        if(global_color_override == ColorOverride::NEVER) return false;
        else if(global_color_override == ColorOverride::ALWAYS) return true;
        return colors_;
    }

    auto getHandle() const noexcept {
        return fd_;
    }

    ~stream() noexcept override;
};

/// @brief Accesses the stream that is linked to stderr.
/// @return Stderr's stream.
extern raw_stream& errs();
/// @brief Accesses the stream that is linked to stdout.
/// @return Stdout's stream.
extern raw_stream& outs();

/// @brief A stream class into an stdio FILE stream.
class standard_file_stream : public raw_stream {
    /// @brief Stream's file handle.
    FILE* file_;
    /// @brief Should the stream call fclose().
    bool close_;
    /// @brief Overriden write implementation to write to the FILE.
    void writeImpl(const char* ptr, size_t size) override;

public:
    /// @brief The basic constructor for this stream class.
    /// @param f Stdio file stream.
    /// @param shouldClose Should this stream take ownership of the file.
    /// @param bufferSize Size of the buffer, 0 to disable.
    ///
    /// Putting the buffer size to 0 does not disable the FILE's buffer.
    standard_file_stream(FILE* f, bool shouldClose,
                         size_t bufferSize = DEFAULT_BUFFER_SIZE) noexcept :
        raw_stream(bufferSize), file_(f), close_(shouldClose) {}

    ~standard_file_stream() noexcept override;
};

/// @brief Provides a raw_stream interface for std::string.
class string_stream : public raw_stream {
    std::string str_;

    void writeImpl(const char* ptr, size_t size) override {
        str_ += std::string_view(ptr, size);
    }

public:
    /// @brief Default constructor.
    string_stream() noexcept : raw_stream(0) {}

    /// @brief Uses the provided string.
    /// @param str String to use.
    string_stream(std::string str) noexcept : str_(std::move(str)) {}

    /// @brief Returns the string and empties the one in the class.
    std::string str() const noexcept {
        return std::move(str_);
    }
};

} // namespace inr

#endif // INERTIA_SUPPORT_STREAM_H
