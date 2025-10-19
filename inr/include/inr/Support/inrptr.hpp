#ifndef INERTIA_INRPTR_HPP
#define INERTIA_INRPTR_HPP

/**
 * @file inr/Support/inrptr.hpp
 * @brief Inertia's pointer classes.
 *
 * This header contains various types of pointers.
 *
 **/

#include "inr/Support/inralloc.hpp"

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

        template<typename... Args>
        unique(allocator* _mem, Args&&... args) : mem(_mem), ptr(_mem->alloc<T>(std::forward<Args>(args)...)){};
        
        unique(allocator* _mem, T* p) noexcept : mem(_mem), ptr(p){};

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

        void reset(allocator* _mem = nullptr, T* p = nullptr) noexcept{
            mem->free(ptr);
            ptr = (_mem && p) ? p : nullptr;
            mem = (_mem && p) ? _mem : nullptr;
        }

        T* release() noexcept{
            T* tmp = ptr;
            ptr = nullptr;
            return tmp;
        }

        T& operator*() noexcept{
            return *ptr;
        }

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

        template<typename... Args>
        unique(allocator* _mem, size_t _count, Args&&... args) : mem(_mem), count(_count), ptr(_mem->alloc_array<T>(_count), std::forward<Args>(args)...){};
        
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

#endif // INERTIA_INRPTR_HPP
