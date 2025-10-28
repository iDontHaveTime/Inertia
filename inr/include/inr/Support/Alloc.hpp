#ifndef INERTIA_ALLOC_HPP
#define INERTIA_ALLOC_HPP

/**
 * @file inr/Support/Alloc.hpp
 * @brief Base class for all Inertia's allocator.
 *
 * This header contains Inertia's base allocator class.
 * The point of this allocator is so that any container can use a variety of allocators.
 * So for example a vector could use an arena allocator and another could use a simple allocator.
 *
 **/

#include <cstddef>
#include <cstdlib>

#include <memory>
#include <type_traits>
#include <utility>

namespace inr{

    template<typename T>
    class unique;

    /**
     * @brief The base allocator class for all allocators.
     *
     * This is the base class of an allocator, Here are some rules for allocators.
     * 1. The allocator is not responsible for freeing the memory allocated (but recommended).
     * 2. The allocator MUST define alloc_raw and free_raw, otherwise nothing works.
     * 3. Thread safety is not required.
     * 4. The allocator does not have to be templated, that's optional.
     * 5. The allocator MUST return nullptr if failed.
     * 6. The allocator MUST work with alloc(...).
     */
    class allocator{
    public:

        allocator() noexcept = default;

        allocator(const allocator&) = delete;
        allocator& operator=(const allocator&) = delete;

        allocator(allocator&&) noexcept = default;
        allocator& operator=(allocator&&) noexcept = default;
        
        /**
         * @brief Allocates an object.
         *
         * This allocates an object and call its constructor with the args provided.
         * It's a universal wrapper for alloc_raw, this works for all allocators.
         *
         * @param args Arguments to pass on to the constructor.
         *
         * @return The pointer to the object.
         */
        template<typename T, typename... Args>
        T* alloc(Args&&... args){
            T* ptr = (T*)alloc_raw(sizeof(T), alignof(T));
            
            if(!ptr) return nullptr;
            std::construct_at<T>(ptr, std::forward<Args>(args)...);
            
            return ptr;
        }

        /**
         * @brief Allocates an array of objects.
         *
         * This allocates an array of objects, constructs them with the provided arguments.
         *
         * @param count The number of elements in the array.
         * @param args Arguments to pass on to the constructor for each element.
         *
         * @return The pointer to the first element in the array.
         */
        template<typename T, typename... Args>
        T* alloc_array(size_t count, Args&&... args){
            T* ptr = (T*)alloc_raw(sizeof(T) * count, alignof(T));
            if(!ptr) return nullptr;

            for(size_t i = 0; i < count; i++){
                new(ptr + i) T(std::forward<Args>(args)...);
            }
            return ptr;
        }

        /**
         * @brief Frees an array of objects.
         *
         * This frees the array and calls the destructor for each object.
         * Providing a wrong pointer or count is UB.
         *
         * @param ptr The pointer to the array of objects.
         * @param count The number of elements in the array.
         */
        template<typename T>
        void free_array(T* ptr, size_t count){
            if(!ptr) return;
            for(size_t i = 0; i < count; i++){
                ptr[i].~T();
            }

            free_raw(ptr, sizeof(T) * count);
        }

        /**
         * @brief Allocates raw memory from the allocator.
         *
         * @param bytes How many bytes to allocate.
         * @param alignment What should their alignment be. Must be a power of 2, otherwise UB.
         *
         * @return The pointer allocated. nullptr if failed.
         */
        virtual void* alloc_raw(size_t bytes, size_t alignment) = 0;

        /**
         * @brief Frees the pointer provided by the same allocator.
         *
         * Providing a pointer from another allocator is definitely UB.
         * Providing the wrong size is definitely once again UB.
         *
         * @param ptr The pointer to free.
         * @param size The size of the pointer provided.
         */
        virtual void free_raw(void* ptr, size_t size) = 0;

        /**
         * @brief Frees the provided object.
         * 
         * Frees the object allocated by THIS allocator.
         * Using objects from another allocator is UB.
         * Putting an array is UB, since this is designed to free objects allocated by alloc().
         *
         * @param ptr The pointer to free.
         */
        template<typename T>
        void free(T* ptr){
            if(!ptr) return;
            if constexpr(std::is_destructible_v<T>){
                ptr->~T();
            }
            free_raw(ptr, sizeof(T));
        }

        /**
         * @brief Used in certain context to mark that an area will be freed after next allocation.
         *
         * Used in stuff like arena allocator to minimize fragmentations.
         *
         * @param ptr The address.
         * @param size The size of the pointer provided.
         *
         */
        virtual void mark_as_might_be_freed(void* ptr, size_t size) noexcept{
            (void)ptr;
            (void)size;
            return;
        }

        /**
         * @brief Used to unmark pointer that has been marked.
         *
         * Used in stuff like arena allocator to minimize fragmentations.
         *
         * @param ptr The address.
         * @param size The size of the pointer provided.
         *
         */
        virtual void unmark_as_might_be_freed(void* ptr, size_t size) noexcept{
            (void)ptr;
            (void)size;
            return;
        }

        /**
         * @brief Returns if the allocator is valid or not.
         * @return Depends on the underlying allocator.
         */
        virtual bool valid() const noexcept = 0;

        virtual ~allocator() noexcept = default;

        /**
         * @brief Makes a new 'inr::unique' pointer.
         *
         * @return The newly made 'inr::unique' class.
         */
        template<typename T, typename... Args>
        requires (!std::is_array_v<T>)
        inr::unique<T> make_unique(Args&&... args);

        /**
         * @brief Makes a new 'inr::unique' array pointer.
         *
         * @return The newly made array 'inr::unique' class.
         */
        template<typename T, typename... Args>
        requires std::is_array_v<T>
        inr::unique<T> make_unique(size_t count, Args&&... args);
    };
    
    /**
     * @brief Basic allocator class, uses malloc underneath.
     *
     * This class is a super basic allocator with basic allocate and free.
     * It technically doesn't use malloc but rather aligned_alloc.
     *
     */
    class basic_allocator : public allocator{
    public:
        basic_allocator() noexcept = default;
        void* alloc_raw(size_t bytes, size_t alignment) noexcept override{
            size_t padded_size = ((bytes + alignment - 1) / alignment) * alignment;
            return std::aligned_alloc(alignment, padded_size);
        }

        void free_raw(void* ptr, size_t) noexcept override{
            std::free(ptr);
        }

        bool valid() const noexcept override{
            return true;
        }
    };

    extern basic_allocator static_allocator;
}

#endif // INERTIA_ALLOC_HPP
