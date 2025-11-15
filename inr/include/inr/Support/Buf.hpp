#ifndef INERTIA_BUF_HPP
#define INERTIA_BUF_HPP

/**
 * @file inr/Support/Buf.hpp
 * @brief Inertia's buffer class.
 *
 * This header contains the buffer classed used by Inertia's classes.
 *
 **/

#include "inr/Defines/CommonTypes.hpp"

#include <cstddef>

namespace inr{

    /**
     * @brief A constexpr-able buffer that can hold types inside efficiently.
     */
    template<typename T>
    class inrbuf{
        T* buf;
        size_t index;
        size_t buffer_size;
    public:
        inrbuf() = delete;

        /**
         * @brief Creates a new buffer from the range provided.
         *
         * @param start The start of the buffer.
         * @param size The total size of the buffer provided.
         */
        constexpr inrbuf(T* start, size_t size) noexcept : buf(start), index(0), buffer_size(size){};

        /**
         * @brief Adds the provided element to the next slot.
         * @return True if flush needed, false if not.
         */
        constexpr bool add(const T& elem) noexcept{
            if(index < buffer_size){
                buf[index] = elem; 
                index++;
                if(index == buffer_size){
                    return true;
                }
            }
            return false;
        }

        /**
         * @brief Resets the buffer's index.
         */
        constexpr void flush() noexcept{
            index = 0;
        }

        /**
         * @brief Returns the underlying pointer to the buffer.
         * @return Pointer to buffer.
         */
        constexpr T* data() noexcept{
            return buf;
        }

        /**
         * @brief Returns the underlying pointer to the buffer, const version.
         * @return Const pointer to buffer.
         */
        constexpr const T* data() const noexcept{
            return buf;
        }

        /**
         * @brief Returns the size of the underlying buffer.
         * @return Size of buffer.
         */
        constexpr size_t size() const noexcept{
            return buffer_size;
        }

        /**
         * @brief Returns the current element index of the buffer.
         * @return Index of element.
         */
        constexpr size_t current() const noexcept{
            return index;
        }

        /**
         * @brief Checks if the buffer's index is 0.
         *
         * @return True if empty.
         */
        constexpr bool empty() const noexcept{
            return index == 0;
        }

        constexpr T& operator[](array_access n) noexcept{
            return buf[n];
        }

        constexpr const T& operator[](array_access n) const noexcept{
            return buf[n];
        }

        constexpr void set_current(size_t new_index) noexcept{
            index = new_index;
        }
    };

}

#endif // INERTIA_BUF_HPP
