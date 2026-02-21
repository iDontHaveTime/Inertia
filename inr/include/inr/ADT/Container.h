#ifndef INERTIA_ADT_CONTAINER_H
#define INERTIA_ADT_CONTAINER_H

#include <inr/Support/Builtin.h>
#include <algorithm>
#include <cstddef>
#include <type_traits>

namespace inr{

    /**
     * @brief An abstract container class.
     */
    template<typename T, typename Y>
    class container{
    protected:
        T* ptr;
        Y length;
    public:
        
        container(const container&) noexcept = delete;
        container& operator=(const container&) noexcept = delete;

        container(container&& o) noexcept : ptr(o.ptr), length(o.length){
            o.ptr = nullptr;
            o.length = 0;
        }

        container& operator=(container&& o) noexcept{
            if(this != &o){
                ptr = o.ptr;
                length = o.length;

                o.ptr = nullptr;
                o.length = 0;
            }
            return *this;
        }

        container() noexcept : ptr(nullptr), length(0){};

        container(T* start, Y size) noexcept : ptr(start), length(size){};

        const T* data() const noexcept{
            return ptr;
        }

        T* data() noexcept{
            return ptr;
        }

        Y capacity() const noexcept{
            return length;
        }

        T& operator[](Y n) noexcept{
            return *(ptr + n);
        }

        const T& operator[](Y n) const noexcept{
            return *(ptr + n);
        }

        ~container() noexcept = default;
    };

    /**
     * @brief A generic container that can resize to the size requested.
     */
    template<typename T>
    class resizeable_container : public container<T, size_t>{
    protected:
        void destroy_this() noexcept{
            if constexpr(!std::is_trivially_copyable_v<T>){
                for(size_t i = this->length; i > 0; i--){
                    this->ptr[i-1].~T();
                }
            }
            ::operator delete(this->ptr);
        }

        void resize_to_goal(size_t n){
            T* new_ptr = nullptr;
            size_t old_len = this->length;

            if(n){
                new_ptr = (T*)::operator new(sizeof(T) * n);

                size_t count = std::min(n, this->length);
                
                if constexpr(std::is_trivially_copyable_v<T>){
                    if(this->ptr)
                        inr::memcpy(
                            new_ptr, 
                            this->ptr, 
                            count * sizeof(T)
                        );
                }
                else{
                    size_t i = 0;
                    try{
                        for(; i < count; i++){
                            new(new_ptr + i) T(std::move(this->ptr[i]));
                        }
                    }
                    catch(...){
                        for(size_t j = i; j > 0; j--)
                            new_ptr[j-1].~T();
                        ::operator delete(new_ptr);
                        throw;
                    }
                }
            }

            destroy_this();

            this->length = n;
            this->ptr = new_ptr;
        }
    public:

        resizeable_container() noexcept : container<T, size_t>(){};

        resizeable_container(resizeable_container&& o) noexcept{
            this->ptr = o.ptr;
            this->length = o.length;

            o.ptr = nullptr;
            o.length = 0;
        }

        resizeable_container& operator=(resizeable_container&& o) noexcept{
            if(this != &o){
                if(this->ptr){
                    destroy_this();
                }

                this->ptr = o.ptr;
                this->length = o.length;

                o.ptr = nullptr;
                o.length = 0;
            }
            return *this;
        }

        resizeable_container(const resizeable_container& o){
            this->ptr = (T*)::operator new(sizeof(T) * o.length);
            this->length = o.length;
            
            if constexpr(std::is_trivially_copyable_v<T>){
                inr::memcpy(
                    this->ptr, 
                    o.ptr, 
                    this->length * sizeof(T)
                );
            }
            else{
                for(size_t i = 0; i < o.length; i++){
                    new(this->ptr + i) T(o.ptr[i]);
                }
            }
        }

        resizeable_container& operator=(const resizeable_container& o){
            if(this != &o){
                if(this->length >= o.length){
                    if constexpr(std::is_trivially_copyable_v<T>){
                        inr::memcpy(this->ptr, o.ptr, o.length * sizeof(T));
                    }
                    else{
                        for(size_t i = 0; i < o.length; i++)
                            this->ptr[i] = o.ptr[i];
                        for(size_t i = o.length; i < this->length; i++)
                            this->ptr[i].~T();
                    }
                    this->length = o.length;
                }
                else{
                    resizeable_container tmp(o);
                    std::swap(this->ptr, tmp.ptr);
                    std::swap(this->length, tmp.length);
                }
            }

            return *this;
        }

        ~resizeable_container() noexcept{
            destroy_this();
        }
    };

}

#endif // INERTIA_ADT_CONTAINER_H
