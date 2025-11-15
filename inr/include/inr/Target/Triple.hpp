#ifndef INERTIA_TRIPLE_HPP
#define INERTIA_TRIPLE_HPP

/**
 * @file inr/Target/Triple.hpp
 * @brief Inertia's target triple class.
 *
 *
 **/

#include "inr/Support/Endian.hpp"

#include <cstdint>

/* Use these following macros for accurate architecture defining. */
#define little_endian 0
#define big_endian 1

#define bit32 0
#define bit64 1
/* -------------------------------------------------------------- */

#define def_arch(unqiue_id, name, is_big_endian, bitness) name = (unqiue_id << 3) | (is_big_endian) \
    | (bitness << 1)

namespace inr{
    /**
     * @brief A target triple class, for example x86_64-linux-gnu.
     */
    class Triple{
    public:
        /**
         * @brief ISA.
         *
         * bit 0 is endian. 1 big, 0 little.
         * bit 1 and 2 are bitness, 0b00 32bit, 0b01 64bit.
         */
        enum arch : uint32_t{
            def_arch(0, x86_64, little_endian, bit64),
            def_arch(1, AArch64, little_endian, bit64)
        };
        enum os : uint8_t{
            linux_ /**< Linux. */
        };
        enum abi : uint8_t{
            gnu /**< Default GNU ABI. */
        };
    private:
        arch _arch;
        os _os;
        abi _abi;
    public:

        /**
         * @brief Default target triple constructor.
         *
         * @param _arch_ ISA.
         * @param _os_ The operating system.
         * @param _abi_ The ABI to use.
         */
        constexpr Triple(arch _arch_, os _os_, abi _abi_) noexcept : _arch(_arch_), _os(_os_), _abi(_abi_){};

        Triple() = delete;

        constexpr Triple(const Triple&) noexcept = default;
        constexpr Triple& operator=(const Triple&) noexcept = default;

        constexpr Triple(Triple&&) noexcept = default;
        constexpr Triple& operator=(Triple&&) noexcept = default;

        constexpr arch get_arch() const noexcept{
            return _arch;
        }

        constexpr os get_os() const noexcept{
            return _os;
        }

        constexpr abi get_abi() const noexcept{
            return _abi;
        }

    };

    constexpr endian get_arch_endian(Triple::arch a) noexcept{
        return ((uint32_t)a & 1) == 1 ? endian::big : endian::little;
    }

    constexpr uint8_t get_arch_bitness(Triple::arch a) noexcept{
        return ((uint32_t)a & 0b110) >> 1;
    }

    constexpr bool is_arch_64bit(Triple::arch a) noexcept{
        uint8_t bitness = get_arch_bitness(a);
        
        return bitness == bit64;
    };
}

#undef little_endian
#undef big_endian

#undef bit32
#undef bit64

#undef def_arch

#endif // INERTIA_TRIPLE_HPP
