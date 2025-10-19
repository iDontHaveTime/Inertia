#ifndef INERTIA_INRCEXPR_HPP
#define INERTIA_INRCEXPR_HPP

#include <climits>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <string>
#include <type_traits>

/**
 * @file inr/Support/inrcexpr.hpp
 * @brief Inertia's constexpr functions.
 *
 * This header contains convenient constexpr functions that Inertia's classes use.
 *
 **/

#if defined(__GNUC__) || defined(__clang__)
#define always_inline_inr [[__gnu__::__always_inline__]]
#else
#define always_inline_inr
#endif


namespace inr{

    /**
     * @brief Returns string length, calls normal strlen if not constexpr.
     * @return String length.
     */
    always_inline_inr
    constexpr size_t cexpr_strlen(const char* _str) noexcept{
        if(std::is_constant_evaluated()){
            size_t len = 0;

            while(_str[len]) len++;

            return len;
        }
        else{
            return strlen(_str);
        }
    }

    /**
     * @brief Returns the width of a type provided, template.
     * @return Width.
     */
    template<typename T>
    always_inline_inr
    consteval size_t widthof() noexcept{
        return sizeof(T)*CHAR_BIT;
    }

    /**
     * @brief Returns the width of a type provided, variable.
     * @return Width.
     */
    template<typename T>
    always_inline_inr
    consteval size_t widthof(const T&) noexcept{
        return sizeof(T)*CHAR_BIT;
    }

    template<typename T>
    [[nodiscard]] always_inline_inr
    constexpr inline bool valid(const T& t) noexcept{
        return t.valid();
    }

    template<typename T>
    [[nodiscard]] always_inline_inr
    constexpr inline bool valid(const T* const t) noexcept{
        return t && t->valid();
    }

    /**
     * @brief Calculates the amount of bytes from bits. Rounds up.
     * @return Bytes rounded up.
     */
    always_inline_inr
    constexpr size_t calculate_bytes_up(size_t bits) noexcept{
        return (bits + 7) >> 3;
    }

    /**
     * @brief Calculates the amount of bytes from bits. Doesn't round.
     * @return Bytes.
     */
    always_inline_inr
    constexpr size_t calculate_bytes_down(size_t bits) noexcept{
        return bits >> 3;
    }

    /**
     * @brief Counts the amount of trailing zeroes of the number.
     * @param x A number with type of unsigned short.
     * @return Trailing zeroes.
     */
    always_inline_inr
    constexpr int short_ctz(unsigned short x) noexcept{
        if(!x) return widthof(x);
        return __builtin_ctzs(x);
    }


    /**
     * @brief Counts the amount of trailing zeroes of the number.
     * @param x A number with type of unsigned int.
     * @return Trailing zeroes.
     */
    always_inline_inr
    constexpr int int_ctz(unsigned int x) noexcept{
        if(!x) return widthof(x);
        return __builtin_ctz(x);
    }

    /**
     * @brief Counts the amount of trailing zeroes of the number.
     * @param x A number with type of unsigned long.
     * @return Trailing zeroes.
     */
    always_inline_inr
    constexpr int long_ctz(unsigned long x) noexcept{
        if(!x) return widthof(x);
        return __builtin_ctzl(x);
    }

    /**
     * @brief Counts the amount of trailing zeroes of the number.
     * @param x A number with type of unsigned long long.
     * @return Trailing zeroes.
     */
    always_inline_inr
    constexpr int long_long_ctz(unsigned long long x) noexcept{
        if(!x) return widthof(x);
        return __builtin_ctzll(x);
    }

    /**
     * @brief Counts the amount of trailing zeroes, auto determines type.
     * @return The amount of trailing zeroes.
     */
    template<typename T>
    constexpr int auto_ctz(T x) noexcept{
        using U = std::make_unsigned_t<T>;
        if(x == 0) return widthof(x);

        if(std::is_constant_evaluated()){
            unsigned count = 0;
            U ux = (U)x;
            while((ux & 1) == 0){
                ux >>= 1; count++;
            }
            return count;
        }
        else if constexpr(sizeof(T) <= sizeof(short)){
            return short_ctz((unsigned short)x);
        }
        else if constexpr(sizeof(T) <= sizeof(int)){
            return int_ctz((unsigned int)x);
        }
        else if constexpr(sizeof(T) <= sizeof(long)){
            return long_ctz((unsigned long)x);
        }
        else if constexpr(sizeof(T) <= sizeof(long long)){
            return long_long_ctz((unsigned long long)x);
        }
        else{
            static_assert(sizeof(T) <= sizeof(long long), "auto_ctz does not support the provided type.");
            return -1;
        }
    }

    /**
     * @brief Power of two detection.
     * @return True if power of 2, false if not.
     */
    template<typename T>
    always_inline_inr
    constexpr bool is_power_of_2(T n) noexcept{
        using U = std::make_unsigned_t<T>;
        return (((U)n & ((U)n - 1)) == 0) && n != 0;
    }

    /**
     * @brief Power of two detection for pointers.
     * @return True if power of 2, false if not.
     */
    template<typename T>
    always_inline_inr
    constexpr bool is_power_of_2(const T* n) noexcept{
        return (((uintptr_t)n & ((uintptr_t)n - 1)) == 0) && n != nullptr;
    }

    /**
     * @brief Checks if the pointer is aligned to the power of 2 provided, templated alignment version.
     * @return True if aligned, false if not.
     */
    template<size_t alignment, typename T>
    always_inline_inr
    constexpr bool is_aligned(const T* ptr) noexcept{
        if constexpr(!is_power_of_2(alignment)) return false;
        return ((uintptr_t)ptr & (alignment-1)) == 0;
    }

    /**
     * @brief Checks if the pointer is aligned to the power of 2 provided.
     * @return True if aligned, false if not.
     */
    template<typename T>
    always_inline_inr
    constexpr bool is_aligned(const T* ptr, size_t alignment) noexcept{
        if(!is_power_of_2(alignment)) return false;
        return ((uintptr_t)ptr & (alignment-1)) == 0;
    }

    /**
     * @brief Template string's base template type.
     *
     * To use in templates you need to do the following:
     * template<basic_template_string tstr> 
     * Then in the function call you do:
     * foo(template_string<tstr> str, ...);
     * Then it should allow the string to be passed in.
     */
    template<auto N>
    struct basic_template_string{
        /**
         * @brief Constructor initializing the string's data.
         */
        constexpr basic_template_string(const char (&str)[N]) noexcept{
            for(size_t i = 0; i < N; i++){
                value[i] = str[i];
            }
        }

        /**
         * @brief String array itself.
         */
        char value[N];

        /**
         * @brief The pointer to the string.
         * @return Pointer to the string.
         */
        constexpr const char* data() const noexcept{
            return value;
        }
        /**
         * @brief The length of the string.
         * @return Length.
         */
        constexpr size_t size() const noexcept{
            return N-1;
        }
    };

    template<size_t N>
    basic_template_string(const char(&)[N]) -> basic_template_string<N>;

    /**
     * @brief A string class using templates, compile-time strings per se.
     */
    template<basic_template_string str>
    struct template_string{
        constexpr const char* data() const noexcept{
            return str.data();
        }
        constexpr size_t size() const noexcept{
            return str.size();
        }
        constexpr size_t length() const noexcept{
            return size();
        }

        constexpr const char* begin() const noexcept{
            return data();
        }
        constexpr const char* end() const noexcept{
            return data() + size();
        }

        constexpr operator std::string_view() const noexcept{
            return std::string_view(data(), size());
        }

        operator std::string() const{
            return std::string(data(), size());
        }

        constexpr bool operator==(const template_string&) const noexcept = default;

        constexpr bool operator==(const std::string_view& sv) const noexcept{
            return sv == std::string_view(data(), size());
        }

        constexpr bool operator!=(const std::string_view& sv) const noexcept{
            return !(*this == sv);
        }


        /**
         * @brief Hash using djb2.
         */
        constexpr uint64_t hash() const noexcept{
            uint64_t h = 5381;
            
            for(char c : *this){
                h = ((h << 5) + h) + c;
            }
            return h;
        }

    };

    /**
     * @brief Rounds x to the nearest multiple of to (yes to, not two).
     *
     * @param x The number to round.
     * @param to The number to round it to.
     *
     * @return The final number.
     */
    template<std::unsigned_integral T>
    always_inline_inr
    constexpr T round_to(T x, T to){
        T remainder = x % to;
        return remainder ? x + (to - remainder) : x;
    }

    /**
     * @brief Rounds x to the nearest multiple of to. Signed version.
     *
     * @param x The number to round.
     * @param to The number to round it to.
     *
     * @return The final number.
     */
    template<std::signed_integral T>
    always_inline_inr
    constexpr T round_to(T x, T to){
        bool negative = x < 0;
        if(negative) x = -x;
        T remainder = x % to;
        T final_val = remainder ? x + (to - remainder) : x;
        return negative ? -final_val : final_val;
    }
}

#undef always_inline_inr

#endif // INERTIA_INRCEXPR_HPP
