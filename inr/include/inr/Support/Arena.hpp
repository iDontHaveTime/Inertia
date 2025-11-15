#ifndef INERTIA_ARENA_HPP
#define INERTIA_ARENA_HPP

/**
 * @file inr/Support/Arena.hpp
 * @brief Inertia's arena allocator.
 *
 * This header contains Inertia's arena allocator class.
 *
 **/

#include "inr/Support/Alloc.hpp"

#include <cstdlib>
#include <cstdint>

#include <memory>

namespace inr{
    /**
     * @brief A simple arena allocator with pre-set size (and alignment).
     */
    template<size_t size, size_t align = 32, inertia_allocator _arena_alloc_ = allocator>
    class arena_allocator : private _arena_alloc_{
        void* arena;
        size_t cur = 0;
    public:

        const void* data() const noexcept{
            return arena;
        }
        void* data() noexcept{
            return arena;
        }

        size_t current() const noexcept{
            return cur;
        }

        template<typename T, typename... Args>
        T* alloc(Args&&... args){
            return _arena_alloc_::template alloc<T>(std::forward<Args>(args)...);
        }

        template<typename T>
        void free(T* ptr){
            return _arena_alloc_::template free<T>(ptr);
        }

        void* alloc_raw(size_t bytes, size_t alignment) noexcept{
            void* ptr = (char*)arena + cur;
            size_t space = size - cur;

            if(std::align(alignment, bytes, ptr, space)){
                cur = (char*)ptr - (char*)arena + bytes;
                return ptr;
            }
            return nullptr;
        }

        void free_raw(void* ptr, size_t sz) noexcept{
            if((uintptr_t)ptr == (((uintptr_t)arena + cur) - sz)){
                cur -= sz;
            }
        }

        bool valid() const noexcept{
            return arena != nullptr;
        }

        arena_allocator(){
            arena = _arena_alloc_::alloc_raw(size, align);
        }

        ~arena_allocator() noexcept{
            _arena_alloc_::free_raw(arena, size);
        }

        arena_allocator(arena_allocator&& o) noexcept{
            arena = o.arena;
            o.arena = nullptr;
        }
        arena_allocator& operator=(arena_allocator&& o) noexcept{
            if(this != &o){
                _arena_alloc_::free_raw(arena, size);
                arena = o.arena;
                o.arena = nullptr;
            }
            return *this;
        }

        void reset() noexcept{
            cur = 0;
        }

        void mark_as_might_be_freed(void* ptr, size_t sz) noexcept{
            free_raw(ptr, sz);
        }

        void unmark_as_might_be_freed(void* ptr, size_t sz) noexcept{
            if(ptr == ((char*)arena + cur)){
                cur += sz;
            }
        }
    };
}

#endif // INERTIA_ARENA_HPP
