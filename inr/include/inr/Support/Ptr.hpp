#ifndef INERTIA_PTR_HPP
#define INERTIA_PTR_HPP

/**
 * @file inr/Support/Ptr.hpp
 * @brief Inertia's pointer classes.
 *
 * This header contains various types of pointers.
 *
 **/

#include "inr/Defines/CommonTypes.hpp"
#include "inr/Support/Alloc.hpp"

#include <type_traits>
#include <utility>

namespace inr{
    /**
     * @brief Inertia's unique ptr replacement.
     */
    template<typename T, inertia_allocator _unique_alloc_ = allocator>
    class unique : private _unique_alloc_{
        T* ptr;

    public:

        unique(const unique&) = delete;
        unique& operator=(const unique&) = delete;

        unique(unique&& other) noexcept : ptr(other.ptr){
            other.ptr = nullptr;
        }

        unique& operator=(unique&& other) noexcept{
            if(this != &other){
                _unique_alloc_::free(ptr);

                ptr = other.ptr;

                other.ptr = nullptr;
            }
            return *this;
        }

        unique() : ptr(_unique_alloc_::template alloc<T>()){};

        /**
         * @brief Allocates a new pointer using the allocator provided.
         *
         * @param args Arguments to pass.
         *
         */
        template<typename... Args>
        unique(Args&&... args) : ptr(_unique_alloc_::template alloc<T>(std::forward<Args>(args)...)){};
        
        /**
         * @brief Assumes ownership of the pointer provided that uses the provided allocator.
         */
        unique(T* p) noexcept : ptr(p){};

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
         * @param p The pointer to use.
         */
        void reset(T* p = nullptr) noexcept{
            _unique_alloc_::free(ptr);
            ptr = p;
        }

        /**
         * @brief Returns the old pointer and removes ownership.
         *
         * @return The old pointer.
         */
        T* release() noexcept{
            T* tmp = ptr;
            ptr = nullptr;
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
    template<typename T, inertia_allocator _unique_alloc_>
    class unique<T[], _unique_alloc_> : private _unique_alloc_{
        T* ptr;
        size_t count;

    public:
        unique() = delete;

        unique(const unique&) = delete;
        unique& operator=(const unique&) = delete;

        unique(unique&& other) noexcept : count(other.count), ptr(other.ptr){
            other.ptr = nullptr;
            other.count = 0;
        }

        unique& operator=(unique&& other) noexcept{
            if(this != &other){
                _unique_alloc_::free_array(ptr, count);

                ptr = other.ptr;
                count = other.count;

                other.ptr = nullptr;
                other.count = 0;
            }
            return *this;
        }

        /**
         * @brief Allocates an array of objects using the allocator provided.
         *
         * @param _count How many objects to allocate.
         * @param args Arguments to pass.
         * 
         */
        template<typename... Args>
        unique(size_t _count, Args&&... args) : count(_count), 
            ptr(_unique_alloc_::template alloc_array<T>(_count, std::forward<Args>(args)...)){};
        
        /**
         * @brief Assumes ownership of the provided array.
         *
         * @param p The array.
         * @param _count How many objects are in the array.
         *
         */
        unique(T* p, size_t _count) noexcept : count(_count), ptr(p){};

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

        void reset(T* p = nullptr, size_t _count = 0) noexcept{
            _unique_alloc_::free_array(ptr, count);
            ptr = p;
            count = (ptr) ? _count : 0;
        }

        T* release() noexcept{
            T* tmp = ptr;
            ptr = nullptr;
            return tmp;
        }

        T& operator[](array_access i) noexcept{
            return ptr[i];
        }

        const T& operator[](array_access i) const noexcept{
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
    inr::unique<T, allocator> allocator::make_unique(Args&&... args) const noexcept{
        return inr::unique<T, allocator>(std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    requires std::is_array_v<T>
    inr::unique<T, allocator> allocator::make_unique(size_t count, Args&&... args) const noexcept{
        return inr::unique<T, allocator>(count, std::forward<Args>(args)...);
    }
}

#endif // INERTIA_PTR_HPP
