#ifndef INERTIA_INRVECTOR_HPP
#define INERTIA_INRVECTOR_HPP

/**
 * @file inr/Support/inrvector.hpp
 * @brief Inertia's vector class.
 *
 * This header contains Inertia's vector class.
 * This is a replacement for STL's std::vector.
 *
 **/

#include "inr/Support/inralloc.hpp"
#include "inr/Support/inriterator.hpp"

#include <cstring>

#include <type_traits>

namespace inr{

    constexpr size_t inr_vec_starting_allocation = 16;

    /**
     * @brief Inertia's replacement to std::vector.
     *
     * This is a custom implementation of a vector to support Inertia's custom allocators.
     */
    template<typename T>
    class inr_vec{
        allocator* mem;
        T* array;
        size_t count;
        size_t allocated;

        /* Returns true on success. */
        bool resize_vector(size_t goal){
            if(!mem) return false;
            if(!array){
                array = (T*)mem->alloc_raw(inr_vec_starting_allocation * sizeof(T), alignof(T));
                if(array){
                    count = 0;
                    allocated = inr_vec_starting_allocation;
                }
                else{
                    return false;
                }
                return true;
            }
            size_t new_size = inr_vec_starting_allocation;
            while(new_size < goal){
                new_size <<= 1;
            }

            mem->mark_as_might_be_freed(array, allocated * sizeof(T));
            T* new_array = (T*)mem->alloc_raw(new_size * sizeof(T), alignof(T));
            if(!new_array){
                mem->unmark_as_might_be_freed(array, allocated * sizeof(T));
                return false;
            }

            try{
                for(size_t i = 0; i < count; i++){
                    new(new_array + i) T(std::move(array[i]));
                }
            } catch(...){
                for(size_t j = 0; j < count; j++){
                    new_array[j].~T();
                }

                mem->unmark_as_might_be_freed(array, allocated * sizeof(T));
                mem->free_raw(new_array, new_size * sizeof(T));
                throw;
            }

            
            for(size_t i = 0; i < count; i++){
                array[i].~T();
            }
            mem->free_raw(array, allocated * sizeof(T));

            array = new_array;
            allocated = new_size;

            return true;
        }
    public:

        /**
         * @brief Basic 'inr_vec' constructor.
         *
         * This constructor allows you to set the underlying allocator.
         *
         * @param _mem The allocator to use inside. 
         */
        inr_vec(allocator* _mem = nullptr) noexcept : array(nullptr), count(0), allocated(0){
            if(_mem){
                mem = _mem;
            }
            else{
                mem = &static_allocator;
            }
        }
        inr_vec(allocator& _mem) noexcept : inr_vec(&_mem){};

        /**
         * @brief Constructor deep copies the vector.
         */
        inr_vec(const inr_vec& other){
            mem = other.mem;
            array = nullptr;
            allocated = 0;
            if(!reserve(other.allocated)) return;
            count = other.count;
            for(size_t i = 0; i < count; i++){
                array[i] = other.array[i];
            }
        }

        /**
         * @brief Operator deep copies the vector.
         */
        inr_vec& operator=(const inr_vec& other){
            if(this == &other) return *this;
            mem = other.mem;
            array = nullptr;
            allocated = 0;
            if(!reserve(other.allocated)) return;
            count = other.count;
            for(size_t i = 0; i < count; i++){
                array[i] = other.array[i];
            }
            return *this;
        }

        /**
         * @brief Constructor transfers vector's ownership.
         */
        inr_vec(inr_vec&& other) noexcept{
            mem = other.mem; // mem does NOT need to be moved.

            allocated = other.allocated;
            other.allocated = 0;

            count = other.count;
            other.count = 0;

            array = other.array;
            other.array = nullptr;
        }

        /**
         * @brief Operator transfers vector's ownership.
         */
        inr_vec& operator=(inr_vec&& other) noexcept{
            if(this == &other) return *this;
            clear(true);
            mem = other.mem;

            allocated = other.allocated;
            other.allocated = 0;

            count = other.count;
            other.count = 0;

            array = other.array;
            other.array = nullptr;

            return *this;
        }

        /**
         * @brief Reserves at least the amount provided.
         *
         * @param n The amount of elements to reserve.
         */
        void reserve(size_t n) noexcept{
            resize_vector(n);
        }

        /**
         * @brief Returns the amount of elements in the vector.
         * @return Amount of elements allocated in the vector.
         */
        size_t size() const noexcept{
            return count;
        }

        /**
         * @brief The amount of elements the vector has space for.
         * @return Amount of space the vector has allocated for elements.
         */
        size_t capacity() const noexcept{
            return allocated;
        }

        /**
         * @brief Returns a read/write reference to the Nth array element. 
         */
        T& operator[](size_t n) noexcept{
            return *(array + n);
        }

        /**
         * @brief Returns a const reference to the Nth array element. 
         */
        const T& operator[](size_t n) const noexcept{
            return *(array + n);
        }

        /**
         * @brief Alternative to operator[].
         *
         * @param n Index to access the element at.
         */
        T& at(size_t n) noexcept{
            return (*this)[n];
        }

        /**
         * @brief Alternative to operator[] const version.
         *
         * @param n Index to access the element at.
         */
        const T& at(size_t n) const noexcept{
            return (*this)[n];
        }

        /**
         * @brief Returns a const reference for the first element.
         */
        const T& front() const noexcept{
            return (*this)[0];
        }

        /**
         * @brief Returns a read/write reference for the first element.
         */
        T& front() noexcept{
            return (*this)[0];
        }

        /**
         * @brief Returns a const reference for the last element.
         */
        const T& back() const noexcept{
            return (*this)[count-1];
        }

        /**
         * @brief Returns a read/write reference for the last element.
         */
        T& back() noexcept{
            return (*this)[count-1];
        }

        /**
         * @brief Removes the last element.
         */
        void pop_back() noexcept{
            if(count) count--;
        }

        /**
         * @brief Clears all elements.
         */
        void clear(bool free_everything = false) noexcept{
            count = 0;
            if(free_everything){
                if(array)
                    mem->free_raw(array, allocated);
                array = nullptr;
                allocated = 0;
            }
        }

        /**
         * @brief Pushes this element to the back of the vector (copy).
         */
        void push_back(const T& _n){
            emplace_back(_n);
        }
        /**
         * @brief Pushes this element to the back of the vector (move).
         */
        void push_back(T&& _n){
            emplace_back(std::move(_n));
        }

        /**
         * @brief Gets the internal allocator.
         */
        allocator* get_allocator() const noexcept{
            return mem;
        }

        /**
         * @brief Constructs T at the end.
         */
        template<typename... Args>
        T& emplace_back(Args&&... args){
            if(count >= allocated){
                if(!resize_vector(allocated << 1)) return array[0];
            }

            T* dest = array + count;
            new(dest) T(std::forward<Args>(args)...);
            count++;
            return *dest;
        }

        /**
         * @brief Returns the array pointer.
         */
        T* data() noexcept{
            return array;
        }

        /**
         * @brief Returns the array pointer, const version.
         */
        const T* data() const noexcept{
            return array;
        }

        /**
         * @brief Begin for the normal iterator.
         */
        array_iterator<T> begin() noexcept{
            return array_iterator<T>(array);
        }

        /**
         * @brief End for the normal iterator.
         */
        array_iterator<T> end() noexcept{
            return array_iterator<T>(array + count);
        }

        /**
         * @brief Begin for the const iterator.
         */
        array_iterator<const T> begin() const noexcept{
            return array_iterator<const T>(array);
        }

        /**
         * @brief End for the const iterator.
         */
        array_iterator<const T> end() const noexcept{
            return array_iterator<const T>(array + count);
        }

        /**
         * @brief Begin for the const iterator explicit.
         */
        array_iterator<const T> cbegin() const noexcept{
            return array_iterator<const T>(array);
        }

        /**
         * @brief End for the const iterator explicit.
         */
        array_iterator<const T> cend() const noexcept{
            return array_iterator<const T>(array + count);
        }

        /**
         * @brief Returns the validity of the vector.
         */
        bool valid() const noexcept{
            return array != nullptr;
        }

        /**
         * @brief This destructor calls the elements' destructors then frees the array.
         */
        ~inr_vec() noexcept{
            if(array){
                if constexpr(std::is_destructible_v<T>){
                    for(T& elem : *this){
                        elem.~T();
                    }
                }
                mem->free_raw(array, allocated);
            }
        }
    };

    /**
     * @brief A vector that stores a certain amount of elements on stack before heap.
     *
     */
    template<typename T, size_t elem_c>
    class inline_vec{
        inr_vec<T> heap;
        size_t cur = 0;
        alignas(T) uint8_t stack[elem_c * sizeof(T)];
        enum class vec_storage{
            STACK, HEAP
        } storage = inline_vec::vec_storage::STACK;
    public:
        
        /* Constructors. */

        /**
         * @brief Basic 'inline_vec' constructor.
         *
         * This constructor allows you to set the underlying allocator.
         *
         * @param _mem The allocator to use inside. 
         */
        inline_vec(allocator* _mem = nullptr) noexcept : heap(_mem){};

        inline_vec(allocator& _mem) noexcept : inline_vec(&_mem){};

        /* Destructor. */

        ~inline_vec() noexcept{
            if(storage == vec_storage::STACK){
                for(size_t i = 0; i < cur; i++){
                    ((T*)stack)[i].~T();
                }
            }
        }

        /* Copies. */

        inline_vec(const inline_vec&) = default;
        inline_vec& operator=(const inline_vec&) = default;

        /* Move. */

        inline_vec(inline_vec&&) = default;
        inline_vec& operator=(inline_vec&&) = default;

        /* Members. */

        /**
         * @brief Reserves at least the amount provided.
         *
         * @param n The amount of elements to reserve.
         */
        void reserve(size_t n) noexcept{
            if(storage != vec_storage::HEAP) return;
            heap.reserve(n);
        }

        /**
         * @brief Returns the amount of elements in the vector.
         * @return Amount of elements allocated in the vector.
         */
        size_t size() const noexcept{
            return storage == vec_storage::HEAP ? heap.size() : cur;
        }

        /**
         * @brief The amount of elements the vector has space for.
         * @return Amount of space the vector has allocated for elements.
         */
        size_t capacity() const noexcept{
            return storage == vec_storage::HEAP ? heap.capacity() : elem_c;
        }

        /**
         * @brief Returns a read/write reference to the Nth array element. 
         */
        T& operator[](size_t n) noexcept{
            return *(data() + n);
        }

        /**
         * @brief Returns a const reference to the Nth array element. 
         */
        const T& operator[](size_t n) const noexcept{
            return *(data() + n);
        }

        /**
         * @brief Alternative to operator[].
         *
         * @param n Index to access the element at.
         */
        T& at(size_t n) noexcept{
            return (*this)[n];
        }

        /**
         * @brief Alternative to operator[] const version.
         *
         * @param n Index to access the element at.
         */
        const T& at(size_t n) const noexcept{
            return (*this)[n];
        }

        /**
         * @brief Returns a const reference for the first element.
         */
        const T& front() const noexcept{
            return (*this)[0];
        }

        /**
         * @brief Returns a read/write reference for the first element.
         */
        T& front() noexcept{
            return (*this)[0];
        }

        /**
         * @brief Returns a const reference for the last element.
         */
        const T& back() const noexcept{
            return (*this)[size()-1];
        }

        /**
         * @brief Returns a read/write reference for the last element.
         */
        T& back() noexcept{
            return (*this)[size()-1];
        }

        /**
         * @brief Removes the last element.
         */
        void pop_back() noexcept{
            if(storage == vec_storage::STACK){
                if(cur) cur--;
            }
            else{
                heap.pop_back();
            }
        }

        /**
         * @brief Clears all elements.
         */
        void clear(bool free_everything = false) noexcept{
            if(storage == vec_storage::STACK){
                cur = 0;
            }
            else{
                heap.clear(free_everything);
            }
        }

        /**
         * @brief Pushes this element to the back of the vector (copy).
         */
        void push_back(const T& _n){
            emplace_back(_n);
        }
        /**
         * @brief Pushes this element to the back of the vector (move).
         */
        void push_back(T&& _n){
            emplace_back(_n);
        }

        /**
         * @brief Gets the internal allocator.
         */
        allocator* get_allocator() const noexcept{
            return heap.get_allocator();
        }

        /**
         * @brief Constructs T at the end.
         */
        template<typename... Args>
        T& emplace_back(Args&&... args){
            if(storage == vec_storage::STACK){
                if(cur == elem_c){
                    storage = vec_storage::HEAP;
                    for(size_t i = 0; i < elem_c; i++){
                        heap.emplace_back(std::move(((T*)stack)[i]));
                    }

                    return heap.emplace_back(std::forward<Args>(args)...);
                }
                else{
                    T* dest = (T*)stack + cur;
                    new(dest) T(std::forward<Args>(args)...);
                    cur++;
                    return *dest;
                }
            }
            else{
                return heap.emplace_back(std::forward<Args>(args)...);
            }
        }

        /**
         * @brief Returns the array pointer.
         */
        T* data() noexcept{
            return storage == vec_storage::HEAP ? heap.data() : (T*)stack;
        }

        /**
         * @brief Returns the array pointer, const version.
         */
        const T* data() const noexcept{
            return storage == vec_storage::HEAP ? heap.data() : (T*)stack;
        }

        /**
         * @brief Begin for the normal iterator.
         */
        array_iterator<T> begin() noexcept{
            return array_iterator<T>(data());
        }

        /**
         * @brief End for the normal iterator.
         */
        array_iterator<T> end() noexcept{
            return array_iterator<T>(data() + size());
        }

        /**
         * @brief Begin for the const iterator.
         */
        array_iterator<const T> begin() const noexcept{
            return array_iterator<const T>(data());
        }

        /**
         * @brief End for the const iterator.
         */
        array_iterator<const T> end() const noexcept{
            return array_iterator<const T>(data() + size());
        }

        /**
         * @brief Begin for the const iterator explicit.
         */
        array_iterator<const T> cbegin() const noexcept{
            return array_iterator<const T>(data());
        }

        /**
         * @brief End for the const iterator explicit.
         */
        array_iterator<const T> cend() const noexcept{
            return array_iterator<const T>(data() + size());
        }

        /**
         * @brief Returns the validity of the vector.
         */
        bool valid() const noexcept{
            return data() != nullptr;
        }
    };
}

#endif // INERTIA_INRVECTOR_HPP
