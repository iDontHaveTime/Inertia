#ifndef INERTIA_PTR_HPP
#define INERTIA_PTR_HPP

/**
 * @file inr/Support/Ptr.hpp
 * @brief Inertia's pointer classes.
 *
 * This header contains various types of pointers.
 *
 **/

#include "inr/Support/Alloc.hpp"

#include <type_traits>
#include <utility>

namespace inr{
    /**
     * @brief Inertia's unique ptr replacement.
     */
    template<typename T>
    class unique{
        T* ptr;
        allocator* mem;

    public:
        unique() = delete;

        unique(const unique&) = delete;
        unique& operator=(const unique&) = delete;

        unique(unique&& other) noexcept : mem(other.mem), ptr(other.ptr){
            other.mem = nullptr;
            other.ptr = nullptr;
        }

        unique& operator=(unique&& other) noexcept{
            if(this != &other){
                mem->free(ptr);

                mem = other.mem;
                ptr = other.ptr;

                other.mem = nullptr;
                other.ptr = nullptr;
            }
            return *this;
        }

        /**
         * @brief Allocates a new pointer using the allocator provided.
         *
         * @param _mem The allocator to use.
         * @param args Arguments to pass.
         *
         */
        template<typename... Args>
        unique(allocator* _mem, Args&&... args) : mem(_mem), ptr(_mem->alloc<T>(std::forward<Args>(args)...)){};
        
        /**
         * @brief Assumes ownership of the pointer provided that uses the provided allocator.
         */
        unique(allocator* _mem, T* p) noexcept : mem(_mem), ptr(p){};

        /**
         * @brief Gets the underlying pointer.
         *
         * @return Pointer.
         */
        T* get() noexcept{
            return ptr;
        }

        /**
         * @brief Gets the underlying pointer. Const version.
         *
         * @return Const pointer.
         */
        const T* get() const noexcept{
            return ptr;
        }

        /**
         * @brief Checks if the pointer is nullptr or not.
         *
         * @return False if nullptr, true if not.
         */
        bool valid() const noexcept{
            return ptr != nullptr;
        }

        /**
         * @brief Uses valid().
         *
         * @return Whatever valid() returns.
         */
        operator bool() const noexcept{
            return valid();
        }

        /**
         * @brief Frees the old pointer, and sets the new one provided.
         *
         * @param _mem The allocator provided pointer uses.
         * @param p The pointer to use.
         */
        void reset(allocator* _mem = nullptr, T* p = nullptr) noexcept{
            mem->free(ptr);
            ptr = (_mem && p) ? p : nullptr;
            mem = (_mem && p) ? _mem : nullptr;
        }

        /**
         * @brief Returns the old pointer and removes ownership.
         *
         * @return The old pointer.
         */
        T* release() noexcept{
            T* tmp = ptr;
            ptr = nullptr;
            mem = nullptr;
            return tmp;
        }

        /**
         * @brief Dereference pointer.
         *
         * @return Object.
         */
        T& operator*() noexcept{
            return *ptr;
        }

        /**
         * @brief Dereference pointer. Const version.
         *
         * @return Const object.
         */
        T& operator*() const noexcept{
            return *ptr;
        }

        T* operator->() noexcept{
            return ptr;
        }

        const T* operator->() const noexcept{
            return ptr;
        }

        ~unique() noexcept{
            reset();
        }

        friend class allocator;
    };

    /**
     * @brief Array version of 'inr::unique'.
     */
    template<typename T>
    class unique<T[]>{
        T* ptr;
        size_t count;
        allocator* mem;

    public:
        unique() = delete;

        unique(const unique&) = delete;
        unique& operator=(const unique&) = delete;

        unique(unique&& other) noexcept : mem(other.mem), count(other.count), ptr(other.ptr){
            other.mem = nullptr;
            other.ptr = nullptr;
            other.count = 0;
        }

        unique& operator=(unique&& other) noexcept{
            if(this != &other){
                mem->free_array(ptr, count);

                mem = other.mem;
                ptr = other.ptr;
                count = other.count;

                other.mem = nullptr;
                other.ptr = nullptr;
                other.count = 0;
            }
            return *this;
        }

        /**
         * @brief Allocates an array of objects using the allocator provided.
         *
         * @param _mem The allocator to use.
         * @param _count How many objects to allocate.
         * @param args Arguments to pass.
         * 
         */
        template<typename... Args>
        unique(allocator* _mem, size_t _count, Args&&... args) : mem(_mem), count(_count), ptr(_mem->alloc_array<T>(_count, std::forward<Args>(args)...)){};
        
        /**
         * @brief Assumes ownership of the provided array.
         *
         * @param _mem The allocator the array used.
         * @param p The array.
         * @param _count How many objects are in the array.
         *
         */
        unique(allocator* _mem, T* p, size_t _count) noexcept : mem(_mem), count(_count), ptr(p){};

        T* get() noexcept{
            return ptr;
        }
        const T* get() const noexcept{
            return ptr;
        }

        bool valid() const noexcept{
            return ptr != nullptr;
        }

        operator bool() const noexcept{
            return valid();
        }

        void reset(allocator* _mem = nullptr, T* p = nullptr, size_t _count = 0) noexcept{
            mem->free_array(ptr, count);
            ptr = (_mem && p) ? p : nullptr;
            mem = (ptr) ? _mem : nullptr;
            count = (ptr) ? _count : 0;
        }

        T* release() noexcept{
            T* tmp = ptr;
            ptr = nullptr;
            return tmp;
        }

        T& operator[](size_t i) noexcept{
            return ptr[i];
        }

        const T& operator[](size_t i) const noexcept{
            return ptr[i];
        }

        size_t size() const noexcept{
            return count;
        }

        ~unique() noexcept{
            reset();
        }

        friend class allocator;
    };

    template<typename T, typename... Args>
    requires (!std::is_array_v<T>)
    inr::unique<T> allocator::make_unique(Args&&... args){
        return inr::unique<T>(this, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    requires std::is_array_v<T>
    inr::unique<T> allocator::make_unique(size_t count, Args&&... args){
        return inr::unique<T>(this, count, std::forward<Args>(args)...);
    }
}

#endif // INERTIA_PTR_HPP
