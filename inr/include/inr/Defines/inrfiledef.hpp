#ifndef INERTIA_INRFILEDEF_HPP
#define INERTIA_INRFILEDEF_HPP

#include <cstdint>
#include <cstdio>

/**
 * @file inr/Defines/inrfiledef.hpp
 * @brief Inertia's defines for files.
 *
 * This header contains Inertia's definitions for anything file related.
 * This defines file related stuff so that Inertia's classes can use them.
 *
 **/

namespace inr::fs{
    /**
     * @brief Ways for opening a file.
     *
     * The format is as follows LSB to MSB, 
     * Bit 0: Writeable,
     * Bit 1-5: Unique,
     * Bit 6: Readable,
     * Bit 7: Always zero.
     */
    enum class OpeningType : uint8_t{
        None = 0, /**< The file isn't open to anything. */
        Write = 0b00000001, /**< Opens the file for writing, creates one if file doesn't exist. */
        Read = 0b01000010, /**< Opens the file for reading, errors if one doesn't exist. */
        Append = 0b00000101, /**< Opens the file at the end, creates one if file doesn't exist. */
        WriteRead = 0b01001001, /**< Opens the file for writing and reading, creates one if file doesn't exist. */
        AppendRead = 0b01010001, /**< Opens the file at the end for appending and reading, creates one if file doesn't exist. */
        ReadWrite = 0b01100001 /**< Opens the file for writing and reading, errors if one doesn't exist. */
    };

    enum class SeekType : uint8_t{
        SeekSet = SEEK_SET, /**< Seeks absolute. */
        SeekCurrent = SEEK_CUR, /**< Seeks from current pointer. */
        SeekEnd = SEEK_END /**< Seeks from the end. */
    };

    /**
     * @brief Checks if the provided file opening mode is available for writing.
     *
     * @return True if writeable.
     */
    constexpr bool opening_type_write(OpeningType ot) noexcept{
        return ((uint8_t)ot & 1);
    }

    /**
     * @brief Checks if the provided file opening mode is available for reading.
     *
     * @return True if readable.
     */
    constexpr bool opening_type_read(OpeningType ot) noexcept{
        return ((uint8_t)ot & 0x40);
    }

    /**
     * @brief Last operation for files open with read/write opening type.
     */
    enum class LastFileOperation : uint8_t{
        None, /**< No last operation (either new or flushed). */
        Writing, /**< Last operation was a write. */ 
        Reading /**< Last operation was a read. */
    };
}

#endif // INERTIA_INRFILEDEF_HPP
