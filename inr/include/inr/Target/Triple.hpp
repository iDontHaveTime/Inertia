#ifndef INERTIA_TRIPLE_HPP
#define INERTIA_TRIPLE_HPP

/**
 * @file inr/Target/Triple.hpp
 * @brief Inertia's target triple class.
 *
 *
 **/

#include <cstdint>

namespace inr{
    /**
     * @brief A target triple class, for example x86_64-linux-gnu.
     */
    class Triple{
    public:
        enum arch : uint8_t{
            x86_64 /**< x86-64 (AMD64) ISA. */
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
}

#endif // INERTIA_TRIPLE_HPP
