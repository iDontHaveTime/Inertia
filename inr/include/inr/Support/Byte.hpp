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

#include <ostream>
#include <istream>

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

        constexpr byte() noexcept = default;

        /**
         * @brief Constructor that sets the value.
         *
         * @param n The value to set it to.
         */
        constexpr byte(uint8_t n) noexcept : val(n){};

        /**
         * @brief Uses std::byte to construct itself.
         *
         * @param n The byte to set it to.
         */
        constexpr byte(std::byte n) noexcept : val((uint8_t)n){};

        constexpr byte(const byte&) noexcept = default;
        constexpr byte& operator=(const byte&) noexcept = default;

        constexpr byte(byte&&) noexcept = default;
        constexpr byte& operator=(byte&&) noexcept = default;

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
         * @brief Writes the raw byte to the Inertia's ostream.
         *
         * Does not write an integer in ascii, writes a raw byte.
         *
         * @param os The stream.
         * @return The stream.
         */
        friend inr_ostream& operator<<(inr_ostream& os, const byte& other) noexcept{
            return os.write((const char*)&other.val, 1);
        }

        /**
         * @brief Writes the raw byte to the STL's ostream.
         *
         * Does not write an integer in ascii, writes a raw byte.
         *
         * @param os The stream.
         * @param other The byte to write.
         *
         * @return The stream.
         */
        friend std::ostream& operator<<(std::ostream& os, const byte& other){
            return os.write((const char*)&other.val, 1);
        }

        /**
         * @brief Reads a raw byte from STL's istream.
         *
         * @param is The stream.
         * @param The byte to write to.
         *
         * @return The stream.
         */
        friend std::istream& operator>>(std::istream& is, byte& to){
            return is.read((char*)&to.val, 1);
        }

        /**
         * @brief Reads a raw byte from Inertia's istream.
         *
         * @param is The stream.
         * @param The byte to write to.
         *
         * @return The stream.
         */
        friend inr::inr_istream& operator>>(inr::inr_istream& is, byte& to){
            return is.read((char*)&to.val, 1);
        }
    };

}

#endif // INERTIA_BYTE_HPP
