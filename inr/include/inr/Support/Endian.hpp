#ifndef INERTIA_ENDIAN_HPP
#define INERTIA_ENDIAN_HPP

/**
 * @file inr/Support/Endian.hpp
 * @brief Inertia's endian related classes and functions.
 *
 *
 **/

#include <cstdint>

#ifdef __BYTE_ORDER__
#define _INR_LITTLE_ENDIAN_ __ORDER_LITTLE_ENDIAN__
#define _INR_BIG_ENDIAN_ __ORDER_BIG_ENDIAN__
#define _INR_BYTE_ORDER_ __BYTE_ORDER__
#else
#define _INR_LITTLE_ENDIAN_ 1234
#define _INR_BIG_ENDIAN_ 4321

#define _INR_BYTE_ORDER_ 1234 // TODO: Make this detect endian, maybe constexpr

#endif

namespace inr{

    /**
     * @brief An enum class containing possible endians.
     */
    enum class endian : uint16_t{
        /**
         * @brief Little endian.
         *
         * For example 4 bytes with the value of 1, aka (uint32_t)1 would look like:
         * 0x01 0x00 0x00 0x00
         */
        little = _INR_LITTLE_ENDIAN_,
        /**
         * @brief Big endian.
         *
         * For example 4 bytes with the value of 1, aka (uint32_t)1 would look like:
         * 0x00 0x00 0x00 0x01
         */
        big = _INR_BIG_ENDIAN_,
        /**
         * @brief Represents this machine's endian.
         */
        native = _INR_BYTE_ORDER_
    };
}

#undef _INR_BYTE_ORDER_
#undef _INR_BIG_ENDIAN_
#undef _INR_LITTLE_ENDIAN_

#endif // INERTIA_ENDIAN_HPP
