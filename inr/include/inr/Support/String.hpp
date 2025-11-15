#ifndef INERTIA_STRING_HPP
#define INERTIA_STRING_HPP

/**
 * @file inr/Support/String.hpp
 * @brief An std::string alternative.
 *
 **/

#include "inr/Support/Alloc.hpp"

#include <cstddef>

namespace inr{

    /**
     * @brief Base for the string class.
     *
     * This class is made so that both normal heap and semi-heap strings are passable.
     * So for example:
     * ```cpp
     * void foo(str& s);
     * ```
     * If we tried to do: 
     * ```cpp
     * foo(istr<32>{"bar"});
     * ```
     * It will error.
     * So to avoid that we use:
     * ```cpp
     * void foo(inrstr&);
     * ```
     * Basically telling to accept any inertia string class, except strref (non modifiable).
     * So now both of these will work:
     * ```cpp
     * foo(istr{"bar"});
     * foo(str{"baz"});
     * ```
     * No errors.
     */
    template<typename c_type, size_t stack_size, inertia_allocator _str_alloc_ = allocator>
    class inrstr : private _str_alloc_{
        char* _str;
        size_t _len;

        c_type on_stack[stack_size];
    public:
        
        inrstr() noexcept : _str(nullptr), _len(0){};

        _str_alloc_ get_allocator() const noexcept{
            return _str_alloc_{};
        }

        ~inrstr() noexcept{
            if(_str){
                _str_alloc_::free_raw(_str, _len);
            }
        }
    };

    /**
     * @brief Inherited class for the heap string.
     */
    template<typename T>
    class str_base : public inrstr<T, (32 - sizeof(inrstr<T, 0>))>{
        using inrstr<T, (32 - sizeof(inrstr<T, 0>))>::inrstr;
    };

    /**
     * @brief A string using 'char' type.
     */
    using str = str_base<char>;

    constexpr size_t x = sizeof(str);

    /**
     * @brief Base for the string that stores some chars inline.
     */
    template<typename T, size_t ss>
    class istr_base : public inrstr<T, ss>{
        using inrstr<T, ss>::inrstr;
    };

    /**
     * @brief Inline string using the 'char' type and stores n characters on stack.
     */
    template<size_t n>
    using istr = istr_base<char, n>;
}

#endif // INERTIA_STRING_HPP
