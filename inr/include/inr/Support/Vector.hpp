#ifndef INERTIA_VECTOR_HPP
#define INERTIA_VECTOR_HPP

/**
 * @file inr/Support/Vector.hpp
 * @brief Inertia's vector class.
 *
 * This header contains Inertia's vector class.
 * This is a replacement for STL's std::vector.
 *
 **/

#include "inr/Defines/CommonTypes.hpp"
#include "inr/Support/Alloc.hpp"
#include "inr/Support/Iterator.hpp"

#include <cstdint>
#include <cstring>

#include <limits>
#include <type_traits>

namespace inr{

    constexpr size_t inr_vec_starting_allocation = 16;
    template<typename T>
    using vector_word = std::conditional_t<(sizeof(T) < sizeof(std::max_align_t)), size_t, uint32_t>;

    /**
     * @brief Inertia's replacement to std::vector.
     *
     * This is a custom implementation of a vector to support Inertia's custom allocators.
     */
    template<typename T, inertia_allocator _vec_alloc_ = allocator>
    class inr_vec : private _vec_alloc_{
    public:
        using word = vector_word<T>; // Can be size_t too.
    private:
        T* array;
        word count;
        word allocated;

        /* Returns true on success. */
        bool resize_vector(word goal){
            if(!array){
                array = (T*)_vec_alloc_::alloc_raw(inr_vec_starting_allocation * sizeof(T), alignof(T));
                if(array){
                    count = 0;
                    allocated = inr_vec_starting_allocation;
                }
                else{
                    return false;
                }
                return true;
            }
            word new_size = inr_vec_starting_allocation;
            while(new_size < goal){
                new_size <<= 1;
            }

            _vec_alloc_::mark_as_might_be_freed(array, allocated * sizeof(T));
            T* new_array = (T*)_vec_alloc_::alloc_raw(new_size * sizeof(T), alignof(T));
            if(!new_array){
                _vec_alloc_::unmark_as_might_be_freed(array, allocated * sizeof(T));
                return false;
            }

            try{
                for(size_t i = 0; i < count; i++){
                    new(new_array + i) T(std::move(array[i]));
                }
            }
            catch(...){
                for(size_t j = 0; j < count; j++){
                    new_array[j].~T();
                }

                _vec_alloc_::unmark_as_might_be_freed(array, allocated * sizeof(T));
                _vec_alloc_::free_raw(new_array, new_size * sizeof(T));
                throw;
            }

            
            for(word i = 0; i < count; i++){
                array[i].~T();
            }
            _vec_alloc_::free_raw(array, allocated * sizeof(T));

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
         */
        inr_vec() noexcept : array(nullptr), count(0), allocated(0){};

        /**
         * @brief Constructor deep copies the vector.
         */
        inr_vec(const inr_vec& other){
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
            clear(true);
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
        void reserve(word n){
            resize_vector(n);
        }

        /**
         * @brief Returns the amount of elements in the vector.
         * @return Amount of elements allocated in the vector.
         */
        word size() const noexcept{
            return count;
        }

        /**
         * @brief The amount of elements the vector has space for.
         * @return Amount of space the vector has allocated for elements.
         */
        word capacity() const noexcept{
            return allocated;
        }

        /**
         * @brief Returns the maximum amount of elements the vector can hold.
         * @return Size type.
         */
        constexpr word max_size() const noexcept{
            return std::numeric_limits<word>::max();
        }

        /**
         * @brief Returns a read/write reference to the Nth array element. 
         */
        T& operator[](array_access n) noexcept{
            return *(array + n);
        }

        /**
         * @brief Returns a const reference to the Nth array element. 
         */
        const T& operator[](array_access n) const noexcept{
            return *(array + n);
        }

        /**
         * @brief Alternative to operator[].
         *
         * @param n Index to access the element at.
         */
        T& at(array_access n) noexcept{
            return (*this)[n];
        }

        /**
         * @brief Alternative to operator[] const version.
         *
         * @param n Index to access the element at.
         */
        const T& at(array_access n) const noexcept{
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
                    _vec_alloc_::free_raw(array, allocated);
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
        _vec_alloc_ get_allocator() const noexcept{
            return _vec_alloc_{};
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
         * @brief Begin for the reverse iterator.
         */
        rarray_iterator<T> rbegin() noexcept{
            return rarray_iterator<T>(array + (count-1));
        }
        
        /**
         * @brief End for the reverse iterator.
         */
        rarray_iterator<T> rend() noexcept{
            return rarray_iterator<T>(array - 1);
        }

        /**
         * @brief Begin for the reverse iterator const version.
         */
        rarray_iterator<const T> rbegin() const noexcept{
            return rarray_iterator<const T>(array + (count-1));
        }
        
        /**
         * @brief End for the reverse iterator const version.
         */
        rarray_iterator<const T> rend() const noexcept{
            return rarray_iterator<const T>(array - 1);
        }

        /**
         * @brief Begin for the reverse iterator const version explicit.
         */
        rarray_iterator<const T> crbegin() const noexcept{
            return rarray_iterator<const T>(array + (count-1));
        }
        
        /**
         * @brief End for the reverse iterator const version explicit.
         */
        rarray_iterator<const T> crend() const noexcept{
            return rarray_iterator<const T>(array - 1);
        }

        /**
         * @brief Returns the validity of the vector.
         */
        bool valid() const noexcept{
            return array != nullptr;
        }

        /**
         * @brief Returns true if empty.
         */
        bool empty() const noexcept{
            return count == 0;
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
                _vec_alloc_::free_raw(array, allocated);
            }
        }

        template<typename, size_t, inertia_allocator>
        friend class inline_vec;
    };

    /**
     * @brief A vector that stores a certain amount of elements on stack before heap.
     *
     */
    template<typename T, size_t elem_c, inertia_allocator _ivec_alloc_ = allocator>
    class inline_vec{
        alignas(T) uint8_t stack[elem_c * sizeof(T)];
        inr_vec<T, _ivec_alloc_> heap;
        enum class vec_storage : uint8_t{
            STACK, HEAP
        } storage = inline_vec::vec_storage::STACK;
    public:
        using word = decltype(heap)::word;
        
        /* Constructors. */

        /**
         * @brief Basic 'inline_vec' constructor.
         *
         * This constructor allows you to set the underlying allocator.
         *
         */
        inline_vec() noexcept = default;

        /* Destructor. */

        ~inline_vec() noexcept{
            if(storage == vec_storage::STACK){
                if constexpr(std::is_destructible_v<T>){
                    for(word i = 0; i < heap.count; i++){
                        ((T*)stack)[i].~T();
                    }
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
        void reserve(word n) noexcept{
            if(storage == vec_storage::STACK && n > elem_c){
                switch_to_heap_storage();
            }
            
            if(storage != vec_storage::STACK) heap.reserve(n);
        }

        /**
         * @brief Returns the amount of elements in the vector.
         * @return Amount of elements allocated in the vector.
         */
        word size() const noexcept{
            return heap.size();
        }

        /**
         * @brief The amount of elements the vector has space for.
         * @return Amount of space the vector has allocated for elements.
         */
        word capacity() const noexcept{
            return storage == vec_storage::HEAP ? heap.capacity() : elem_c;
        }

        /**
         * @brief Returns the maximum amount of elements the vector can hold.
         * @return Size type.
         */
        constexpr word max_size() const noexcept{
            return std::numeric_limits<word>::max();
        }

        /**
         * @brief Returns a read/write reference to the Nth array element. 
         */
        T& operator[](array_access n) noexcept{
            return *(data() + n);
        }

        /**
         * @brief Returns a const reference to the Nth array element. 
         */
        const T& operator[](array_access n) const noexcept{
            return *(data() + n);
        }

        /**
         * @brief Alternative to operator[].
         *
         * @param n Index to access the element at.
         */
        T& at(array_access n) noexcept{
            return (*this)[n];
        }

        /**
         * @brief Alternative to operator[] const version.
         *
         * @param n Index to access the element at.
         */
        const T& at(array_access n) const noexcept{
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
            heap.pop_back();
        }

        /**
         * @brief Clears all elements.
         */
        void clear(bool free_everything = false) noexcept{
            if(storage == vec_storage::STACK){
                heap.count = 0;
            }
            else{
                heap.clear(free_everything);
                if(free_everything){
                    storage = vec_storage::STACK;
                }
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
        _ivec_alloc_ get_allocator() const noexcept{
            return _ivec_alloc_{};
        }

    private:
        void switch_to_heap_storage(){
            if(storage != vec_storage::STACK) return;
            size_t count = heap.count;
            heap.count = 0;
            storage = vec_storage::HEAP;
            for(size_t i = 0; i < count; i++){
                heap.emplace_back(std::move(((T*)stack)[i]));
            }
        }
    public:

        /**
         * @brief Constructs T at the end.
         */
        template<typename... Args>
        T& emplace_back(Args&&... args){
            if(storage == vec_storage::STACK){
                if(heap.count == elem_c){
                    switch_to_heap_storage();

                    return heap.emplace_back(std::forward<Args>(args)...);
                }
                else{
                    T* dest = (T*)stack + heap.count;
                    new(dest) T(std::forward<Args>(args)...);
                    heap.count++;
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
         * @brief Begin for the reverse iterator.
         */
        rarray_iterator<T> rbegin() noexcept{
            return rarray_iterator<T>(data() + (size()-1));
        }
        
        /**
         * @brief End for the reverse iterator.
         */
        rarray_iterator<T> rend() noexcept{
            return rarray_iterator<T>(data() - 1);
        }

        /**
         * @brief Begin for the reverse iterator const version.
         */
        rarray_iterator<const T> rbegin() const noexcept{
            return rarray_iterator<const T>(data() + (size()-1));
        }
        
        /**
         * @brief End for the reverse iterator const version.
         */
        rarray_iterator<const T> rend() const noexcept{
            return rarray_iterator<const T>(data() - 1);
        }

        /**
         * @brief Begin for the reverse iterator const version explicit.
         */
        rarray_iterator<const T> crbegin() const noexcept{
            return rarray_iterator<const T>(data() + (size()-1));
        }
        
        /**
         * @brief End for the reverse iterator const version explicit.
         */
        rarray_iterator<const T> crend() const noexcept{
            return rarray_iterator<const T>(data() - 1);
        }

        /**
         * @brief Returns the validity of the vector.
         */
        bool valid() const noexcept{
            return data() != nullptr;
        }

        /**
         * @brief Returns true if empty.
         */
        bool empty() const noexcept{
            return size() == 0;
        }
    };
}

#endif // INERTIA_VECTOR_HPP
