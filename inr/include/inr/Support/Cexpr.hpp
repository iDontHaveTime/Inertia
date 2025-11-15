#ifndef INERTIA_CEXPR_HPP
#define INERTIA_CEXPR_HPP

/**
 * @file inr/Support/Cexpr.hpp
 * @brief Inertia's constexpr functions.
 *
 * This header contains convenient constexpr functions that Inertia's classes use.
 *
 **/

#include "inr/Defines/Attribute.hpp"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
 
#include <concepts>
#include <string>
#include <type_traits>

namespace inr{

    /**
     * @brief Returns string length, calls normal strlen if not constexpr.
     * @return String length.
     */
    _inr_always_inline_
    constexpr size_t cexpr_strlen(const char* _str) noexcept{
        #if _inr_check_builtin_(__builtin_strlen)
            return __builtin_strlen(_str);
        #else
        if(std::is_constant_evaluated()){
            
            size_t len = 0;

            while(_str[len]) len++;

            return len;
        }
        else{
            return strlen(_str);
        }
        #endif
    }

    /**
     * @brief Returns the width of a type provided, template.
     * @return Width.
     */
    template<typename T>
    _inr_always_inline_
    consteval size_t widthof() noexcept{
        return sizeof(T)*CHAR_BIT;
    }

    /**
     * @brief Returns the width of a type provided, variable.
     * @return Width.
     */
    template<typename T>
    _inr_always_inline_
    consteval size_t widthof(const T&) noexcept{
        return sizeof(T)*CHAR_BIT;
    }

    template<typename T>
    [[nodiscard]] _inr_always_inline_
    constexpr inline bool valid(const T& t) noexcept{
        return t.valid();
    }

    template<typename T>
    [[nodiscard]] _inr_always_inline_
    constexpr inline bool valid(const T* const t) noexcept{
        return t && t->valid();
    }

    /**
     * @brief Calculates the amount of bytes from bits. Rounds up.
     * @return Bytes rounded up.
     */
    _inr_always_inline_
    constexpr size_t calculate_bytes_up(size_t bits) noexcept{
        return (bits + 7) >> 3;
    }

    /**
     * @brief Calculates the amount of bytes from bits. Doesn't round.
     * @return Bytes.
     */
    _inr_always_inline_
    constexpr size_t calculate_bytes_down(size_t bits) noexcept{
        return bits >> 3;
    }

    /**
     * @brief Counts the amount of trailing zeroes of the number.
     * @param x A number with type of unsigned short.
     * @return Trailing zeroes.
     */
    _inr_always_inline_
    constexpr int short_ctz(unsigned short x) noexcept{
        if(!x) return widthof(x);
        return __builtin_ctzs(x);
    }


    /**
     * @brief Counts the amount of trailing zeroes of the number.
     * @param x A number with type of unsigned int.
     * @return Trailing zeroes.
     */
    _inr_always_inline_
    constexpr int int_ctz(unsigned int x) noexcept{
        if(!x) return widthof(x);
        return __builtin_ctz(x);
    }

    /**
     * @brief Counts the amount of trailing zeroes of the number.
     * @param x A number with type of unsigned long.
     * @return Trailing zeroes.
     */
    _inr_always_inline_
    constexpr int long_ctz(unsigned long x) noexcept{
        if(!x) return widthof(x);
        return __builtin_ctzl(x);
    }

    /**
     * @brief Counts the amount of trailing zeroes of the number.
     * @param x A number with type of unsigned long long.
     * @return Trailing zeroes.
     */
    _inr_always_inline_
    constexpr int long_long_ctz(unsigned long long x) noexcept{
        if(!x) return widthof(x);
        return __builtin_ctzll(x);
    }

    /**
     * @brief Counts the amount of trailing zeroes, auto determines type.
     * @return The amount of trailing zeroes. -1 if unsupported.
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
        else if constexpr(sizeof(T) <= 2){
            if constexpr(sizeof(short) == 2){
                return short_ctz((uint16_t)x);
            }
            else if constexpr(sizeof(int) == 2){
                return int_ctz((uint16_t)x);
            }
            else{
                return -1;
            }
        }
        else if constexpr(sizeof(T) <= 4){
            if constexpr(sizeof(int) == 4){
                return int_ctz((uint32_t)x);
            }
            else if constexpr(sizeof(long) == 4){
                long_ctz((uint32_t)x);
            }
            else{
                return -1;
            }
        }
        else if constexpr(sizeof(T) <= 8){
            if constexpr(sizeof(long) == 8){
                return long_ctz((uint64_t)x);
            }
            else if constexpr(sizeof(long long) == 8){
                return long_long_ctz((uint64_t)x);
            }
            else{
                return -1;
            }
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
    _inr_always_inline_
    constexpr bool is_power_of_2(T n) noexcept{
        using U = std::make_unsigned_t<T>;
        return (((U)n & ((U)n - 1)) == 0) && n != 0;
    }

    /**
     * @brief Power of two detection for pointers.
     * @return True if power of 2, false if not.
     */
    template<typename T>
    _inr_always_inline_
    constexpr bool is_power_of_2(const T* n) noexcept{
        return (((uintptr_t)n & ((uintptr_t)n - 1)) == 0) && n != nullptr;
    }

    /**
     * @brief Checks if the pointer is aligned to the power of 2 provided, templated alignment version.
     * @return True if aligned, false if not.
     */
    template<size_t alignment, typename T>
    _inr_always_inline_
    constexpr bool is_aligned(const T* ptr) noexcept{
        if constexpr(!is_power_of_2(alignment)) return false;
        return ((uintptr_t)ptr & (alignment-1)) == 0;
    }

    /**
     * @brief Checks if the pointer is aligned to the power of 2 provided.
     * @return True if aligned, false if not.
     */
    template<typename T>
    _inr_always_inline_
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
    _inr_always_inline_
    constexpr T round_to(T x, T to) noexcept{
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
    _inr_always_inline_
    constexpr T round_to(T x, T to) noexcept{
        bool negative = x < 0;
        if(negative) x = -x;
        T remainder = x % to;
        T final_val = remainder ? x + (to - remainder) : x;
        return negative ? -final_val : final_val;
    }

    /**
     * @brief Swaps byte places. 2 bytes.
     *
     * @param x Bytes to swap.
     *
     * @return Swapped bytes.
     */
    _inr_always_inline_
    constexpr uint16_t bswap16(uint16_t x) noexcept{
        return __builtin_bswap16(x);
    }

    /**
     * @brief Swaps byte places. 4 bytes.
     *
     * @param x Bytes to swap.
     *
     * @return Swapped bytes.
     */
    _inr_always_inline_
    constexpr uint32_t bswap32(uint32_t x) noexcept{
        return __builtin_bswap32(x);
    }

    /**
     * @brief Swaps byte places. 8 bytes.
     *
     * @param x Bytes to swap.
     *
     * @return Swapped bytes.
     */
    _inr_always_inline_
    constexpr uint64_t bswap64(uint64_t x) noexcept{
        return __builtin_bswap64(x);
    }

    /**
     * @brief Swaps byte places. Auto bytes.
     *
     * @param n Bytes to swap.
     *
     * @return Swapped bytes.
     */
    template<std::integral T>
    _inr_always_inline_
    constexpr T bswap(T n) noexcept{
        if constexpr(sizeof(T) == 1){
            return n;
        }
        else if constexpr(sizeof(T) == 2){
            return bswap16(n);
        }
        else if constexpr(sizeof(T) == 4){
            return bswap32(n);
        }
        else if constexpr(sizeof(T) == 8){
            return bswap64(n);
        }
        else{
            return n;
        }
    }

    /**
     * @brief Only byte swaps if condition is true.
     */
    template<std::integral T>
    _inr_always_inline_
    constexpr T bswap_if(T n, bool to_bswap) noexcept{
        return to_bswap ? bswap<T>(n) : n;
    }

    /**
     * @brief Max size of all templates provided.
     *
     * The biggest size template out of all of the ones provided.
     *
     * @return Biggest size.
     */
    template<typename... Ts>
    consteval size_t max_size_of_ts() noexcept{
        size_t cur = 1;
        ((cur = cur < sizeof(Ts) ? sizeof(Ts) : cur), ...);
        return cur;
    }

    /**
     * @brief Max align of all templates provided.
     *
     * The biggest alignment template out of all of the ones provided.
     *
     * @return Biggest alignment.
     */
    template<typename... Ts>
    consteval size_t max_align_of_ts() noexcept{
        size_t cur = 1;
        ((cur = cur < alignof(Ts) ? alignof(Ts) : cur), ...);
        return cur;
    }

    /**
     * @brief Equivalent of std::isspace but constexpr.
     *
     * @param c Character to check.
     *
     * @return True if is a separator.
     */
    _inr_always_inline_
    constexpr bool is_separator(char c) noexcept{
        switch(c){
            case '\t':
                [[fallthrough]];
            case '\n':
                [[fallthrough]];
            case '\v':
                [[fallthrough]];
            case '\f':
                [[fallthrough]];
            case '\r':
                [[fallthrough]];
            case ' ':
                return true;
            default:
                return false;
        }
    }

    /**
     * @brief Bit to shifts, so like 32bit to shift 5. Must be a power of 2.
     * @return Bitshift.
     */
    template<std::unsigned_integral shT>
    _inr_always_inline_
    constexpr shT bit_to_shift(shT width) noexcept{
        if(!is_power_of_2(width)) return 0;
        return auto_ctz(width);
    }

    /**
     * @brief Converts a char32_t to a utf byte array.
     *
     * @param buf The buffer to write to, for safety must be at least 4 bytes.
     * @param c The character to convert.
     *
     * @return Bytes written to array.
     */
    [[nodiscard]] _inr_always_inline_
    constexpr size_t char32_to_utf8(char buf[4], char32_t c) noexcept{
        uint32_t codepoint = c;

        if(codepoint > 0x10FFFF || (codepoint >= 0xD800 && codepoint <= 0xDFFF)){
            return 0;
        }

        if(codepoint <= 0x7F){
            buf[0] = char(codepoint);
            return 1;
        }
        else if(codepoint <= 0x7FF){
            buf[0] = char(0xC0 | ((codepoint >> 6) & 0x1F));
            buf[1] = char(0x80 | (codepoint & 0x3F));
            return 2;
        }
        else if(codepoint <= 0xFFFF){
            buf[0] = char(0xE0 | ((codepoint >> 12) & 0x0F));
            buf[1] = char(0x80 | ((codepoint >> 6) & 0x3F));
            buf[2] = char(0x80 | (codepoint & 0x3F));
            return 3;
        }
        else{
            buf[0] = char(0xF0 | ((codepoint >> 18) & 0x07));
            buf[1] = char(0x80 | ((codepoint >> 12) & 0x3F));
            buf[2] = char(0x80 | ((codepoint >> 6) & 0x3F));
            buf[3] = char(0x80 | (codepoint & 0x3F));
            return 4;
        }
    }

    /**
     * @brief Converts a char16_t unit to utf byte array.
     *
     * @param buf The buffer to write to, recommended size of 3.
     * @param cu The character.
     *
     * @return Bytes written.
     */
    [[nodiscard]] _inr_always_inline_
    constexpr size_t char16_unit_to_utf8(char buf[4], char16_t cu) noexcept{
        if(cu >= 0xD800 && cu <= 0xDFFF){
            return 0;
        }
        return char32_to_utf8(buf, (char32_t)cu);
    }

    /**
     * @brief Converts a char16_t pair to a utf byte array.
     *
     * @param buf The buffer to write to, size must be at least 4.
     * @param high High surrogate.
     * @param low Low surrogate.
     *
     * @return Bytes written.
     */
    [[nodiscard]] _inr_always_inline_
    constexpr size_t char16_surrogate_pair_to_utf8(char buf[4], char16_t high, char16_t low) noexcept{
        if(high < 0xD800 || high > 0xDBFF || low  < 0xDC00 || low  > 0xDFFF){
            return 0;
        }

        char32_t codepoint = 0x10000 + ((char32_t(high) - 0xD800) << 10) + (char32_t(low) - 0xDC00);

        return char32_to_utf8(buf, codepoint);
    }
}

#undef _inr_always_inline_

#endif // INERTIA_CEXPR_HPP
