#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <vector>

namespace Inertia{
    template<typename T>
    class Allocator{
        std::vector<T> allocated;
    public:

        Allocator() = default;

        Allocator(size_t reserve){
            allocated.reserve(reserve);
        }

        operator std::vector<T>&() noexcept{
            return allocated;
        }

        operator const std::vector<T>&() const noexcept{
            return allocated;
        }

        Allocator(Allocator&) = delete;
        Allocator& operator=(Allocator&) = delete;

        Allocator(Allocator&& rhs) : allocated(std::move(rhs.allocated)){}
        
        Allocator& operator=(Allocator&& rhs){
            if(this != &rhs){
                allocated = std::move(rhs.allocated);
            }
            return *this;
        }

        template<typename... Args>
        T* alloc(Args&&... args){
            allocated.emplace_back(std::forward<Args>(args)...);
            return &allocated.back();
        }

        ~Allocator() = default;
    };
}

#endif // ALLOCATOR_HPP
