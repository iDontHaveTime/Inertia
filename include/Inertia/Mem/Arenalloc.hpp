#ifndef INERTIA_ARENALLOC_HPP
#define INERTIA_ARENALLOC_HPP

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <vector>

/** Heads up
  *  This is not thread safe, this should not be used in global context.
  *  If used in global/shared context, this should be thread local.
  *  The tracked pointers should outlive this arena allocator.
**/

namespace Inertia{
    struct ArenaAllocPtr{
        void* heap_ptr;
        void** saved_ptr;
        void (*destructor)(void*);

        ArenaAllocPtr() noexcept : saved_ptr(nullptr), destructor(nullptr){};
        ArenaAllocPtr(void** ptr, void* heap, void (*ds)(void*)) noexcept : heap_ptr(heap), saved_ptr(ptr), destructor(ds){};
        ~ArenaAllocPtr() = default;
    };
    class ArenaAlloc{
        void* arena = nullptr;
        size_t current : ((sizeof(size_t)*8)-1);
        bool des_call : 1;
        size_t size = 0;
        std::vector<ArenaAllocPtr> track;

        template<typename T>
        static void destroy(void* p){
            static_cast<T*>(p)->~T();
        }
        inline void call_destructors(bool nullify = true){
            for(auto it = track.rbegin(); it != track.rend(); it++){
                if(it->destructor) it->destructor(it->heap_ptr);
            }
            if(nullify){
                track.clear();
            }
        }
    public:
        ArenaAlloc() noexcept : current(0), des_call(true){};
        ArenaAlloc(const ArenaAlloc&) = delete;
        ArenaAlloc& operator=(const ArenaAlloc&) = delete;

        // ALLOCATES THE ARENA, RETURNS TRUE IF ERROR FOUND
        inline bool allocate_size(size_t _size){
            if(_size <= size) return false; // not an error
            if(!arena){
                arena = malloc(_size);
                if(arena){
                    size = _size;
                }
                else{
                    return true;
                }
            }
            else{
                void* temp = realloc(arena, _size);
                if(!temp){
                    return true;
                }
                
                char* oldArena = (char*)arena;
                size_t oldSize = size;

                ptrdiff_t diff = (char*)temp - oldArena;

                size = _size;
                arena = temp;

                if(diff == 0) return false;
                if(!track.empty()){
                    for(ArenaAllocPtr& ptr : track){
                        if(ptr.saved_ptr){
                            char* p = (char*)(*ptr.saved_ptr);
                            if(p >= oldArena && p < oldArena + oldSize)
                                *ptr.saved_ptr = p + diff;
                        }
                    }
                }
            }
            return false;
        }

        template<typename T, typename... Args>
        inline void alloc(size_t size, T*& to_track, Args&&... args){
            alloc(size, (void**)&to_track, alignof(T), std::is_trivially_destructible<T>::value ? nullptr : &destroy<T>);
            new(to_track) T(std::forward<Args>(args)...);
        }

        // returns how many pointers were cleared
        inline size_t reset(bool call_des = true){
            size_t cleared = 0;
            if(call_des){
                call_destructors(false);
                cleared = track.size();
            }
            track.clear();
            current = 0;
            return cleared;
        }

        void alloc(size_t s, void** to_track, size_t align = alignof(void*), void (*dstr)(void*) = nullptr){ /* not alignof(max_align_t) on purpose */
            if((align == 0) || (align & (align - 1)) != 0){
                *to_track = nullptr;
                return;
            }

            if(current & (align - 1)){
                current = (current + align - 1) & ~(align - 1);
            }

            if(current + s >= size){
                size_t cpys = size;
                if(cpys == 0) cpys = 1;
                while(cpys <= current + s){
                    cpys <<= 1;
                }
                if(allocate_size(cpys)){
                    *to_track = nullptr;
                    return;
                }
            }

            *to_track = ((char*)arena + current);
            track.emplace_back(to_track, *to_track, dstr);
            current += s;
        }

        // This doesnt actually remove it from memory (yet), only calls destructor (if true)
        template<typename T>
        void remove(T*& ptr, bool call_dtor = true){
            std::vector<ArenaAllocPtr>::iterator it = std::find_if(track.begin(), track.end(), [ptr](const ArenaAllocPtr& obj){
                return obj.heap_ptr == ptr;
            });

            if(it == track.end()){
                return;
            }
            
            if(call_dtor) if(it->destructor) it->destructor(it->heap_ptr);

            ptr = nullptr;

            track.erase(it);
        }
        
        // caller checks if valid
        ArenaAlloc(size_t _size) noexcept{
            des_call = true;
            current = 0;
            arena = malloc(_size);
            if(arena){
                size = _size;
            }
        }

        operator bool() const noexcept{
            return arena != nullptr;
        }

        const void* heap() const noexcept{
            return arena;
        }

        inline void destructors(bool state) noexcept{
            des_call = state;
        }

        inline bool used_heap() const noexcept{
            return arena != nullptr;
        }

        ~ArenaAlloc() noexcept{
            if(des_call){
                call_destructors(false);
            }
            if(arena){
                free(arena);
            }
        }
    };
}

#endif // INERTIA_ARENALLOC_HPP
