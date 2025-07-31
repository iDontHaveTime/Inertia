#ifndef INERTIA_ARENALLOC_HPP
#define INERTIA_ARENALLOC_HPP

#include <cstddef>
#include <cstdlib>
#include <vector>

/** Heads up
  *  This is not thread safe, this should not be used in global context.
  *  If used in global/shared context, this should be thread local.
  *  The tracked pointers should outlive this arena allocator.
**/

namespace Inertia{

    template<typename T>
    class ArenaPointer{
        T* ptr;
        std::vector<ArenaPointer<void>>* parent;
        size_t index : 63;
        bool stack_alloc : 1;
        void (*destructor)(T*);

    public:
        
        ArenaPointer() noexcept : ptr(nullptr), parent(nullptr), index(0), stack_alloc(false), destructor(nullptr){};
        ArenaPointer(T* tptr, decltype(parent) _parent, void (*dstr)(T*), size_t idx) noexcept : ptr(tptr), parent(_parent), index(idx), stack_alloc(false), destructor(dstr){};

        ArenaPointer(const ArenaPointer& rhs) noexcept{
            ptr = rhs.ptr;
            destructor = rhs.destructor;
            parent = rhs.parent;
            index = rhs.index;
            stack_alloc = true;
        }
        ArenaPointer& operator=(const ArenaPointer&) = default;

        ArenaPointer(ArenaPointer&& rhs) noexcept : ptr(rhs.ptr), parent(rhs.parent), index(rhs.index), destructor(rhs.destructor){
            rhs.ptr = nullptr;
            rhs.parent = nullptr;
            rhs.destructor = nullptr;
            rhs.index = 0;
        }
        ArenaPointer& operator=(ArenaPointer&& rhs){
            if(this != &rhs){
                ptr = rhs.ptr;
                parent = rhs.parent;
                destructor = (decltype(destructor))rhs.destructor;
                index = rhs.index;


                rhs.ptr = nullptr;
                rhs.parent = nullptr;
                rhs.destructor = nullptr;
            }
            return *this;
        }

        T* operator->() noexcept{
            get();
            return ptr;
        }

        ArenaPointer<T> copy_for_map() noexcept{
            return {
                .ptr = ptr,
                .destructor = destructor,
                .parent = parent,
                .index = index,
                .stack_alloc = true,
            };
        }

        operator T*() noexcept{
            get();
            return ptr;
        }

        inline const T* raw() const noexcept{
            return ptr;
        }

        inline const T* get() noexcept{
            if(index < parent->size()){
                ptr = (T*)((*parent)[index].raw());
                return ptr;
            }
            else{
                return nullptr;
            }
        }

        inline size_t get_index() const noexcept{
            return index;
        }

        ~ArenaPointer() noexcept{
            if(!ptr || stack_alloc) return;
            if(destructor){
                destructor(ptr);
            }
        }

        inline operator bool() const noexcept{
            return ptr != nullptr;
        }

        friend class ArenaAlloc;
    };

    class ArenaAlloc{
        void* arena = nullptr;
        size_t current = 0, cursize = 0;
        std::vector<ArenaPointer<void>> ptrs;

        template<typename T>
        static void destroy(void* p) noexcept{
            static_cast<T*>(p)->~T();
        }

        size_t reach_size(size_t size) const noexcept{
            if(size < cursize) return cursize;
            size_t cpy = cursize;
            if(cpy == 0) cpy = 1;
            while(size >= cpy){
                cpy <<= 1;
            }
            return cpy;
        }

    public:

        template<typename T, typename... Args>
        inline ArenaPointer<T> alloc(Args&&... args){
            T* tptr = (T*)_allocate(sizeof(T), alignof(T));
            new(tptr) T(std::forward<Args>(args)...);
            ptrs.emplace_back(tptr, &ptrs, std::is_trivially_destructible<T>::value ? nullptr : &destroy<T>, ptrs.size());
            // arena pointer uses T* meaning everything is a pointer there
            // meaning it should not matter to cast ArenaPointer to another template type
            return {(ArenaPointer<T>&)ptrs.back()};
        }

        void* _allocate(size_t size, size_t align) noexcept{
            if(size == 0 || align == 0) return nullptr;

            if((align == 0) || (align & (align - 1)) != 0){
                return nullptr;
            }

            if(current & (align - 1)){
                current = (current + align - 1) & ~(align - 1);
            }

            if(current + size >= cursize){
                size_t to = reach_size(current + size);
                if(reserve(to)) return nullptr;
            }

            size_t oldc = current;
            current += size;

            return (char*)arena + oldc;
        }

        // returns true on failure
        bool reserve(size_t size) noexcept{
            if(cursize >= size) return false;
            if(size - cursize < 1024){
                size += 1024 - (size - cursize);
            }
            if(!arena){
                arena = malloc(size);
                if(arena){
                    cursize = size;
                }
                else{
                    return true;
                }
            }
            else{
                void* temp = realloc(arena, size);
                if(!temp){
                    return true;
                }

                char* oldArena = (char*)arena;

                ptrdiff_t diff = (char*)temp - oldArena;

                cursize = size;
                arena = temp;

                if(diff == 0){
                    return false;
                }

                if(!ptrs.empty()){
                    for(ArenaPointer<void>& p : ptrs){
                        p.ptr = ((char*)(p.ptr)) + diff;
                    }
                }
            }
            return false;
        }

        const void* heap() const noexcept{
            return arena;
        }

        ArenaAlloc() = default;
        ArenaAlloc(size_t size) noexcept{
            reserve(size);
        }

        inline void slime_pointers() noexcept{
            ptrs.clear();
        }

        ~ArenaAlloc(){
            if(!arena) return;
            if(!ptrs.empty()){
                slime_pointers();
            }
            free(arena);
            arena = nullptr;
        }
    };
}

#endif // INERTIA_ARENALLOC_HPP
