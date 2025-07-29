#ifndef ARENALLOC_HPP
#define ARENALLOC_HPP

#include <cstddef>
#include <cstdlib>
#include <vector>

namespace Inertia{
    class ArenaAlloc{
        void* arena = nullptr;
        size_t current = 0;
        size_t size = 0;
        std::vector<void**> track;
    public:
        ArenaAlloc() = default;
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
                ptrdiff_t diff = (ptrdiff_t)temp - (ptrdiff_t)arena;
                arena = temp;
                if(diff == 0) return false;
                if(!track.empty()){
                    for(void** ptr : track){
                        if(*ptr){
                            *ptr = (char*)(*ptr) + diff;
                        }
                    }
                }
            }
            return false;
        }

        template<typename T>
        inline void alloc(size_t size, T*& to_track, size_t align = alignof(void*)){
            alloc(size, (void**)&to_track, align);
        }

        void alloc(size_t s, void** to_track, size_t align = alignof(void*)){ /* not alignof(max_align_t) on purpose */
            if((align == 0) || (align & (align - 1)) != 0){
                *to_track = nullptr;
                return;
            }

            if(current & (align - 1)){
                current = (current + align - 1) & ~(align - 1);
            }

            if(current + s >= size){
                size_t cpys = size;
                while(cpys <= current + s){
                    cpys <<= 1;
                }
                if(allocate_size(cpys)){
                    *to_track = nullptr;
                    return;
                }
            }

            track.push_back(to_track);
            *to_track = ((char*)arena + current);
            current += s;
        }
        
        // caller checks if valid
        ArenaAlloc(size_t _size) noexcept{
            arena = malloc(_size);
            if(arena){
                size = _size;
            }
        }

        operator bool() const noexcept{
            return arena != nullptr;
        }

        ~ArenaAlloc() noexcept{
            if(arena){
                free(arena);
            }
        }
    };
}

#endif // ARENALLOC_HPP
