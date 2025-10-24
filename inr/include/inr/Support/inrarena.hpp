#ifndef INERTIA_INRARENA_HPP
#define INERTIA_INRARENA_HPP

/**
 * @file inr/Support/inrarena.hpp
 * @brief Inertia's arena allocator.
 *
 * This header contains Inertia's arena allocator class.
 *
 **/

#include "inr/Support/inralloc.hpp"

namespace inr{
    /**
     * @brief A simple arena allocator with pre-set size (and alignment).
     */
    template<size_t size, size_t align = 32>
    class arena_allocator : public allocator{
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

        void* alloc_raw(size_t bytes, size_t alignment) noexcept override{
            void* ptr = (char*)arena + cur;
            size_t space = size - cur;

            if(std::align(alignment, bytes, ptr, space)){
                cur = (char*)ptr - (char*)arena + bytes;
                return ptr;
            }
            return nullptr;
        }

        void free_raw(void* ptr, size_t sz) noexcept override{
            if((uintptr_t)ptr == (((uintptr_t)arena + cur) - sz)){
                cur -= sz;
            }
        }

        bool valid() const noexcept override{
            return arena != nullptr;
        }

        arena_allocator(){
            arena = static_allocator.alloc_raw(size, align);
        }

        ~arena_allocator() noexcept{
            static_allocator.free_raw(arena, size);
        }

        arena_allocator(arena_allocator&& o) noexcept{
            arena = o.arena;
            o.arena = nullptr;
        }
        arena_allocator& operator=(arena_allocator&& o) noexcept{
            if(this != &o){
                static_allocator.free_raw(arena, size);
                arena = o.arena;
                o.arena = nullptr;
            }
            return *this;
        }

        void reset() noexcept{
            cur = 0;
        }

        void mark_as_might_be_freed(void* ptr, size_t sz) noexcept override{
            free_raw(ptr, sz);
        }

        void unmark_as_might_be_freed(void* ptr, size_t sz) noexcept override{
            if(ptr == ((char*)arena + cur)){
                cur += sz;
            }
        }
    };
}

#endif // INERTIA_INRARENA_HPP
