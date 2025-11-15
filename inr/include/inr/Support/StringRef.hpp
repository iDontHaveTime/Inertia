#ifndef INERTIA_STRINGREF_HPP
#define INERTIA_STRINGREF_HPP

/**
 * @file inr/Support/StringRef.hpp
 * @brief A string_view alternative.
 *
 **/

#include "inr/Defines/CommonTypes.hpp"
#include "inr/Support/Cexpr.hpp"
#include "inr/Support/Iterator.hpp"
#include "inr/Support/Stream.hpp"

#include <cstddef>

#include <algorithm>
#include <limits>

namespace inr{

    /**
     * @brief Base class for the 'strref' class.
     *
     * This class is designed for storing a string, but can be used for other purposes.
     */
    template<typename c_type>
    class base_strref{
        const c_type* _str;
        size_t _len;
    public:
        
        constexpr base_strref() noexcept : _str(nullptr), _len(0){};

        explicit constexpr
        base_strref(const c_type* _cstr) noexcept : _str(_cstr), 
            _len(cexpr_strlen(_cstr) >> bit_to_shift(sizeof(c_type))){};

        constexpr base_strref(const c_type* ptr, decltype(_len) len) noexcept
            : _str(ptr), _len(len){}

        constexpr base_strref(const base_strref&) noexcept = default;
        constexpr base_strref& operator=(const base_strref&) noexcept = default;

        constexpr base_strref(base_strref&&) noexcept = default;
        constexpr base_strref& operator=(base_strref&&) noexcept = default;

        constexpr const c_type& operator[](array_access n) const noexcept{
            return *(_str + n);
        }

        constexpr const c_type& at(array_access n) const noexcept{
            return (*this)[n];
        }

        constexpr const c_type* data() const noexcept{
            return _str;
        }

        constexpr const c_type& front() const noexcept{
            return *_str;
        }

        constexpr const c_type& back() const noexcept{
            return *(_str + (_len-1));
        }

        constexpr size_t length() const noexcept{
            return _len;
        }

        constexpr size_t size() const noexcept{
            return _len;
        }

        constexpr size_t max_size() const noexcept{
            return std::numeric_limits<size_t>::max();
        }

        constexpr bool empty() const noexcept{
            return _len == 0;
        }

        constexpr array_iterator<const c_type> begin() const noexcept{
            return array_iterator<const c_type>(_str);
        }

        constexpr array_iterator<const c_type> end() const noexcept{
            return array_iterator<const c_type>(_str + _len);
        }

        constexpr array_iterator<const c_type> cbegin() const noexcept{
            return array_iterator<const c_type>(_str);
        }

        constexpr array_iterator<const c_type> cend() const noexcept{
            return array_iterator<const c_type>(_str + _len);
        }

        constexpr rarray_iterator<const c_type> rbegin() const noexcept{
            if(empty()) return {};
            return rarray_iterator<const c_type>(_str + (_len-1));
        }

        constexpr rarray_iterator<const c_type> rend() const noexcept{
            if(empty()) return {};
            return rarray_iterator<const c_type>(_str - 1);
        }

        constexpr rarray_iterator<const c_type> crbegin() const noexcept{
            if(empty()) return {};
            return rarray_iterator<const c_type>(_str + (_len-1));
        }

        constexpr rarray_iterator<const c_type> crend() const noexcept{
            if(empty()) return {};
            return rarray_iterator<const c_type>(_str - 1);
        }

        template<ostream_t T>
        friend T& operator<<(T& os, const base_strref& sref){
            if constexpr(sizeof(c_type) == 1){
                return os.write((const char*)sref.data(), sref.size());
            }
            else{
                for(decltype(_len) i = 0; i < sref._len; i++){
                    os.put(*(sref._str + i));
                }
                return os;
            }
        }
        
        constexpr auto operator<=>(const base_strref& other) const noexcept{
            return std::lexicographical_compare_three_way(
                _str, _str + _len, 
                other._str, other._str + other._len
            );
        }

        constexpr ~base_strref() noexcept = default;
    };

    /**
     * @brief A non-owning string class.
     */
    using strref = base_strref<char>;

    /**
     * @brief A wchar_t non-owning string class.
     */
    using wstrref = base_strref<wchar_t>;

    /**
     * @brief A char8_t non-owning string class.
     */
    using u8strref = base_strref<char8_t>;

    /**
     * @brief A char16_t non-owning string class.
     */
    using u16strref = base_strref<char16_t>;

    /**
     * @brief A char32_t non-owning string class.
     */
    using u32strref = base_strref<char32_t>;
}

#endif // INERTIA_STRINGREF_HPP
