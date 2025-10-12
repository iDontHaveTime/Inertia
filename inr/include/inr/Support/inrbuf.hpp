#ifndef INERTIA_INRBUF_HPP
#define INERTIA_INRBUF_HPP

#include <cstddef>

/**
 * @file inr/Support/inrbuf.hpp
 * @brief Inertia's buffer class.
 *
 * This header contains the buffer classed used by Inertia's classes.
 *
 **/


namespace inr{

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
        inrbuf(T* start, size_t size) noexcept : buf(start), index(0), buffer_size(size){};

        /**
         * @brief Adds the provided element to the next slot.
         * @return True if flush needed, false if not.
         */
        bool add(const T& elem) noexcept{
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
        void flush() noexcept{
            index = 0;
        }

        /**
         * @brief Returns the underlying pointer to the buffer.
         * @return Pointer to buffer.
         */
        T* data() noexcept{
            return buf;
        }

        /**
         * @brief Returns the underlying pointer to the buffer, const version.
         * @return Const pointer to buffer.
         */
        const T* data() const noexcept{
            return buf;
        }

        /**
         * @brief Returns the size of the underlying buffer.
         * @return Size of buffer.
         */
        size_t size() const noexcept{
            return buffer_size;
        }

        /**
         * @brief Returns the current element index of the buffer.
         * @return Index of element.
         */
        size_t current() const noexcept{
            return index;
        }
    };

}

#endif // INERTIA_INRBUF_HPP
