#ifndef INERTIA_CONSTSTR_HPP
#define INERTIA_CONSTSTR_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace Inertia{
    constexpr size_t cexpr_strlen(const char* str) noexcept{
        size_t len = 0;
        while(str[len]){
            len++;
        }
        return len;
    }

    template<int I, typename T, typename Y, typename U, typename F>
    struct string_integer_chosen{
        using type = T;
    };

    template<typename T, typename Y, typename U, typename F>
    struct string_integer_chosen<0, T, Y, U, F>{
        using type = T;
    };

    template<typename T, typename Y, typename U, typename F>
    struct string_integer_chosen<1, T, Y, U, F>{
        using type = Y;
    };

    template<typename T, typename Y, typename U, typename F>
    struct string_integer_chosen<2, T, Y, U, F>{
        using type = U;
    };

    template<typename T, typename Y, typename U, typename F>
    struct string_integer_chosen<3, T, Y, U, F>{
        using type = F;
    };

    constexpr int string_type_length(size_t length) noexcept{
        if(length < UINT8_MAX){
            return 0;
        }
        else if(length < UINT16_MAX){
            return 1;
        }
        else if (length < UINT32_MAX){
            return 2;
        }
        else{
            return 3;
        }
    }

    template<size_t I>
    using string_integer_type = string_integer_chosen<string_type_length(I), uint8_t, uint16_t, uint32_t, uint64_t>::type;

    using hash_type = size_t;

    template<auto N>
    class string_template_type_base{
    public:
        char value[N];
        using length_type = string_integer_type<N>;

        constexpr string_template_type_base(const char (&str)[N]){
            std::copy_n(str, N, value);
        }
        constexpr const char* data() const noexcept{
            return value;
        }
        constexpr length_type length() const noexcept{
            return N-1;
        }
        constexpr auto size() const noexcept{
            return length();
        }
    };

    template<auto N>
    using string_template_type = string_template_type_base<N>;
    

    template<string_template_type str>
    class t_str{
    public:
        using length_type = decltype(str)::length_type;

        constexpr decltype(str) get_template() const noexcept{
            return str;
        }

        constexpr t_str() noexcept = default;

        constexpr const char* data() const noexcept{
            return str.data();
        }

        constexpr hash_type hash() const noexcept{
            size_t h = 0;
            for(size_t i = 0; i < str.length(); i++){
                h = h * 31 + (size_t)str.data()[i];
            }
            return h;
        }

        constexpr auto length() const noexcept{
            return str.length();
        }
        constexpr auto size() const noexcept{
            return length();
        }
    };

    class const_str{
    public:
        using length_type = size_t;
    private:
        const char* str;
        const length_type len;
    public:

        constexpr const_str(const char* _str) noexcept : str(_str), len(std::is_constant_evaluated() ? cexpr_strlen(_str) : strlen(_str)){};

        constexpr const char* data() const noexcept{
            return str;
        }

        constexpr hash_type hash() const noexcept{
            size_t h = 0;
            for(size_t i = 0; i < len; i++){
                h = h * 31 + (size_t)str[i];
            }
            return h;
        }

        constexpr size_t length() const noexcept{
            return len;
        }
        constexpr size_t size() const noexcept{
            return length();
        }

        constexpr ~const_str() noexcept = default;
    };

    class t_string_entry{
        size_t hash;
    public:

        template<string_template_type T>
        constexpr t_string_entry(const t_str<T>& str) noexcept : hash(str.hash()){};

        constexpr size_t get_hashed() const noexcept{
            return hash;
        }
    };

    template<typename Value>
    class t_string_map_item{
    public:
        t_string_entry first;
        Value second;

        template<string_template_type T>
        constexpr t_string_map_item(const t_str<T>& _first, Value _second) : first(_first), second(_second){};
    };

    template<typename T>
    using t_string_map = t_string_map_item<T>[];

    template<typename T>
    constexpr size_t sizeof_map(const T& map) noexcept{
        return sizeof(map) / sizeof(*map);
    }

    //constexpr t_string_map_item<int> map[] = {
    //    {t_str<"Hello">(), 0}, {t_str<"olleH">(), 1}
    //};

    // a function to test those
    // static void func(const const_str& _str){
    //     constexpr t_str<"Hello"> str;
    //     size_t l = str.length();
    //     const char* dt = str.data();
    //     size_t hash = str.hash();
    // }
}

#endif // INERTIA_CONSTSTR_HPP
