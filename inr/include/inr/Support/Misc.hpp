#ifndef INERTIA_MISC_HPP
#define INERTIA_MISC_HPP

/**
 * @file inr/Support/Misc.hpp
 * @brief Inertia's miscellaneous classes.
 *
 * This header contains classes that might or might not be used.
 *
 **/

#include "inr/Support/Byte.hpp"
#include "inr/Support/Cexpr.hpp"

#include <cstddef>

namespace inr{

    /**
     * @brief This class will be the amount of bytes that you gave it.
     */
    template<size_t bytes>
    class raw_storage{
        byte _data[bytes];
    public:
        constexpr raw_storage() noexcept = default;
        constexpr ~raw_storage() noexcept = default;
        constexpr raw_storage(const raw_storage&) noexcept = default;
        constexpr raw_storage& operator=(const raw_storage&) noexcept = default;
        constexpr raw_storage(raw_storage&&) noexcept = default;
        constexpr raw_storage& operator=(raw_storage&&) noexcept = default;

        constexpr byte* data() noexcept{
            return _data;
        }
        constexpr const byte* data() const noexcept{
            return _data;
        }
        constexpr size_t size() const noexcept{
            return bytes;
        }
    };

    /**
     * @brief Becomes the size of the largest type in the typename list.
     */
    template<typename... Ts>
    class max_template_storage_non_aligned{
        raw_storage<max_size_of_ts<Ts...>()> storage;
    public:
        constexpr max_template_storage_non_aligned() noexcept = default;
        constexpr ~max_template_storage_non_aligned() noexcept = default;
        constexpr max_template_storage_non_aligned(const max_template_storage_non_aligned&) noexcept = default;
        constexpr max_template_storage_non_aligned& operator=(const max_template_storage_non_aligned&) noexcept = default;
        constexpr max_template_storage_non_aligned(max_template_storage_non_aligned&&) noexcept = default;
        constexpr max_template_storage_non_aligned& operator=(max_template_storage_non_aligned&&) noexcept = default;

        constexpr byte* data() noexcept{
            return storage.data();
        }
        constexpr const byte* data() const noexcept{
            return storage.data();
        }
        constexpr size_t size() const noexcept{
            return storage.size();
        }
    };

    /**
     * @brief Becomes the size of the largest type in the typename list. Aligned version.
     */
    template<typename... Ts>
    class max_template_storage_aligned{
        alignas(max_align_of_ts<Ts...>()) raw_storage<max_size_of_ts<Ts...>()> storage;
    public:
        constexpr max_template_storage_aligned() noexcept = default;
        constexpr ~max_template_storage_aligned() noexcept = default;
        constexpr max_template_storage_aligned(const max_template_storage_aligned&) noexcept = default;
        constexpr max_template_storage_aligned& operator=(const max_template_storage_aligned&) noexcept = default;
        constexpr max_template_storage_aligned(max_template_storage_aligned&&) noexcept = default;
        constexpr max_template_storage_aligned& operator=(max_template_storage_aligned&&) noexcept = default;

        constexpr byte* data() noexcept{
            return storage.data();
        }
        constexpr const byte* data() const noexcept{
            return storage.data();
        }
        constexpr size_t size() const noexcept{
            return storage.size();
        }
    };

    template<typename... Ts>
    using max_storage = max_template_storage_aligned<Ts...>;

    template<typename... Ts>
    using max_storage_unaligned = max_template_storage_non_aligned<Ts...>;
}

#endif // INERTIA_MISC_HPP
