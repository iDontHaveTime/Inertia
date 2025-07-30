#ifndef INERTIA_AUTOPOINTER_HPP
#define INERTIA_AUTOPOINTER_HPP

#include <cstdint>

namespace Inertia{
    // Just auto frees pointer
    enum class AutoPointerType : uint8_t{
        Single,
        Array,
    };

    template<typename T, AutoPointerType type>
    class AutoPointer{
        T* ptr;

        inline void destruct(){
            if(ptr){
                if(type == AutoPointerType::Array){
                    delete[] ptr;
                }
                else{
                    delete ptr;
                }
            }
            ptr = nullptr;
        }
    public:

        operator const T*() const noexcept{
            return ptr;
        }

        operator T*() noexcept{
            return ptr;
        }

        inline const T* get() const noexcept{
            return ptr;
        }

        inline T* get() noexcept{
            return ptr;
        }

        inline T* release() noexcept{
            T* old = ptr;
            ptr = nullptr;
            return old;
        }

        inline void reset(T* newptr = nullptr) noexcept{
            destruct();
            ptr = newptr;
        }

        AutoPointer(const AutoPointer&) = delete;
        AutoPointer& operator=(const AutoPointer&) = delete;

        AutoPointer() noexcept : ptr(nullptr){};
        AutoPointer(T* _mem) noexcept : ptr(_mem){};

        AutoPointer(AutoPointer&& other) noexcept : ptr(other.ptr){
            other.ptr = nullptr;
        }

        AutoPointer& operator=(AutoPointer&& rhs) noexcept{
            if(this != &rhs){
                destruct();
                ptr = rhs.ptr;
                rhs.ptr = nullptr;
            }
            return *this;
        }

        ~AutoPointer() noexcept{
            destruct();
        }
    };
}

#endif // INERTIA_AUTOPOINTER_HPP
