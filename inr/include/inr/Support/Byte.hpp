#ifndef INERTIA_BYTE_HPP
#define INERTIA_BYTE_HPP

/**
 * @file inr/Support/Byte.hpp
 * @brief Inertia's byte class.
 *
 * This header contains a byte class.
 *
 **/

#include "inr/Support/Stream.hpp"

#include <cstddef>
#include <cstdint>

namespace inr{

    /**
     * @brief Inertia's byte class.
     *
     * Its just a byte, aka 8 bits.
     * Can be constexpr.
     */
    class byte{
        /**
         * @brief The value stored in the byte.
         */
        uint8_t val;
    public:

        constexpr byte() noexcept : val(0){};
        constexpr ~byte() noexcept = default;

        /**
         * @brief Constructor that sets the value.
         *
         * @param n The value to set it to.
         */
        constexpr byte(uint8_t n) noexcept : val(n){};

        /**
         * @brief Uses std::byte to construct.
         *
         * @param n The byte to set it to.
         */
        constexpr byte(std::byte n) noexcept : val((uint8_t)n){};

        constexpr byte(const byte&) noexcept = default;
        constexpr byte& operator=(const byte&) noexcept = default;

        constexpr byte(byte&&) noexcept = default;
        constexpr byte& operator=(byte&&) noexcept = default;

        constexpr void set_bit(bool to, uint8_t bit) noexcept{
            if(to){
                val |= (1 << bit);
            }
            else{
                val &= ~(1 << bit);
            }
        }

        constexpr bool get_bit(uint8_t bit) const noexcept{
            return (val & (1 << bit)) != 0;
        }

        constexpr byte& operator=(uint8_t other) noexcept{
            val = other;
            return *this;
        }

        constexpr byte& operator=(std::byte other) noexcept{
            val = (uint8_t)other;
            return *this;
        }
        
        constexpr auto operator<=>(const byte&) const noexcept = default;

        explicit constexpr operator uint8_t() const noexcept{
            return val;
        }

        explicit constexpr operator std::byte() const noexcept{
            return (std::byte)val;
        }

        constexpr byte operator~() const noexcept{
            return byte(~val);
        }

        constexpr byte& operator&=(const byte& other) noexcept{
            val &= other.val;
            return *this;
        }
        
        constexpr byte& operator|=(const byte& other) noexcept{
            val |= other.val;
            return *this;
        }

        constexpr byte& operator^=(const byte& other) noexcept{
            val ^= other.val;
            return *this;
        }

        constexpr byte& operator<<=(const byte& other) noexcept{
            val <<= other.val;
            return *this;
        }

        constexpr byte& operator>>=(const byte& other) noexcept{
            val >>= other.val;
            return *this;
        }

        constexpr byte operator&(const byte& other) const noexcept{
            return byte(*this) &= other;
        }

        constexpr byte operator|(const byte& other) const noexcept{
            return byte(*this) |= other;
        }

        constexpr byte operator^(const byte& other) const noexcept{
            return byte(*this) ^= other;
        }

        constexpr byte operator<<(const byte& other) const noexcept{
            return byte(*this) <<= other;
        }

        constexpr byte operator>>(const byte& other) const noexcept{
            return byte(*this) >>= other;
        }

        constexpr byte operator++(int) noexcept{
            byte cpy(*this);
            ++(*this);
            return cpy;
        }

        constexpr byte& operator++() noexcept{
            val++;
            return *this;
        }

        constexpr byte operator--(int) noexcept{
            byte cpy(*this);
            --(*this);
            return cpy;
        }

        constexpr byte& operator--() noexcept{
            val--;
            return *this;
        }

        constexpr byte& operator+=(const byte& other) noexcept{
            val += other.val;
            return *this;
        }

        constexpr byte& operator-=(const byte& other) noexcept{
            val -= other.val;
            return *this;
        }

        constexpr byte& operator*=(const byte& other) noexcept{
            val *= other.val;
            return *this;
        }

        constexpr byte& operator/=(const byte& other) noexcept{
            val /= other.val;
            return *this;
        }

        constexpr byte& operator%=(const byte& other) noexcept{
            val %= other.val;
            return *this;
        }

        constexpr byte operator+(const byte& other) const noexcept{
            return byte(*this) += other;
        }

        constexpr byte operator-(const byte& other) const noexcept{
            return byte(*this) -= other;
        }

        constexpr byte operator*(const byte& other) const noexcept{
            return byte(*this) *= other;
        }

        constexpr byte operator/(const byte& other) const noexcept{
            return byte(*this) /= other;
        }

        constexpr byte operator%(const byte& other) const noexcept{
            return byte(*this) %= other;
        }

        constexpr byte operator-() const noexcept{
            return byte(-val);
        }

        constexpr byte operator+() const noexcept{
            return byte(val);
        }

        /**
         * @brief Writes the raw byte to the  ostream.
         *
         * Does not write an integer in ascii, writes a raw byte.
         *
         * @param os The stream.
         * @param other The byte to write.
         *
         * @return The stream.
         */
        template<ostream_t T>
        friend T& operator<<(T& os, const byte& other) noexcept{
            return os.write((const char*)&other.val, 1);
        }

        /**
         * @brief Reads a raw byte from the istream.
         *
         * @param is The stream.
         * @param to The byte to write to.
         *
         * @return The stream.
         */
        template<istream_t T>
        friend T& operator>>(T& is, byte& to){
            return is.read((char*)&to.val, 1);
        }
    };

}

#endif // INERTIA_BYTE_HPP
