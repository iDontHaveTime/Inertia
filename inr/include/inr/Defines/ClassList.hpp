#ifndef INERTIA_CLASSLIST_HPP
#define INERTIA_CLASSLIST_HPP

/**
 * @file inr/Defines/ClassList.hpp
 * @brief Header for Inertia's classes.
 *
 * This header is useless, it just lists classes.
 * This header should NOT use doxygen comments to explain classes.
 * 
 *
 **/

#include <cstddef>
#include <cstdint>

namespace inr{

    /* Header: inr/Defines/inrapis.hpp */
    
    /* A simple enum class for available platform types. 
     - Such as POSIX, Windows, and Standard (for non-Windows, or POSIX).
    */
    enum class APIs : uint8_t;

    /* Filesystem namespace. Header: inr/Defines/inrfiledef.hpp */
    namespace fs{
        /* Opening type of a file. Such as reading, writing, or appending (more exist). */
        enum class OpeningType : uint8_t;
        /* Seek types. Just represent SEEK_CUR, SEEK_SET, and SEEK_END with an enum class. */
        enum class SeekType : uint8_t;

        /* Returns true if the 'inr::fs::OpeningType' provided is writeable. */
        constexpr bool opening_type_write(OpeningType ot) noexcept;
        /* Returns true if the 'inr::fs::OpeningType' provided is readable. */
        constexpr bool opening_type_read(OpeningType ot) noexcept;

        /* Last file operation, could be Write, Read, or None. */
        enum class LastFileOperation : uint8_t;
    }

    /* Header: inr/IR/inrcontext.hpp */

    /* IR Context class. Stores IR module's global state. */
    class inrContext;

    /* Header: inr/Support/inralloc.hpp */

    /* Base class for all Inertia's Allocators. */
    class allocator;

    /* Inherits allocator, uses malloc and free inside. Compatible with them too. */
    class basic_allocator;

    /* Header: inr/Support/inrarena.hpp */

    /* A simple arena allocator, also inherits allocator. */
    template<size_t size, size_t align = 32>
    class arena_allocator;

    /* Header: inr/Support/inrbuf.hpp */

    /* A simple buffer class with just a pointer, size, and index tracking. */
    template<typename T>
    class inrbuf;

    /* Header: inr/Support/inrbyte.hpp */

    /* Super straightforward, just a byte, a uint8_t underneath. */
    class byte;

    /* Skipped header: inr/Support/inrcexpr.hpp 
     - Reason: Too much to include.
    */

    /* Header: inr/Support/inrendian.hpp */

    /* This enum class represents endians, which are little and big. Also includes native. */
    enum class endian : uint16_t;

    /* Header: inr/Support/inrfile.hpp */

    /* The in-memory loaded file. */
    class inr_mem_file;
    /* Handle for posix files. fd + buffer. */
    class inr_posix_handle;
    /* Handle for windows files. handle + buffer. */
    class inr_windows_handle;
    /* The cross-platform handle that abstracts all the platform specifics. */
    struct inr_file_handle;
    /* The wrapper for the 'inr::inr_file_handle' class. */
    class inrfile;

    /* Header: inr/Support/inrint.hpp */

    /* An arbitrary precision integer class. */
    class inrint;

    /* Header: inr/Support/inriterator.hpp */

    /* A generic pointer iterator. */
    template<typename T>
    struct array_iterator;

    /* Header: inr/Support/inrptr.hpp */

    /* A unique pointer class that supports Inertia's allocator class. */
    template<typename T>
    class unique;

    /* Header: inr/Support/inrstream.hpp */

    /* Base class for Inertia's streams. dfopt is default opening type. */
    template<fs::OpeningType dfopt>
    class inr_iostream;
    /* Inertia's output stream. */
    class inr_ostream;
    /* Inertia's input stream. */
    class inr_istream;

    /* Header: inr/Support/inrvector.hpp */

    /* A vector, a simple straightforward vector class, supports Inertia's allocators. */
    template<typename T>
    class inr_vec;
    /* Vector but stores some elements on stack first before going heap.*/
    template<typename T, size_t elem_c>
    class inline_vec;
}

#endif // INERTIA_CLASSLIST_HPP
