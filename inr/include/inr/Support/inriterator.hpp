#ifndef INERTIA_INRITERATOR_HPP
#define INERTIA_INRITERATOR_HPP

/**
 * @file inr/Support/inriterator.hpp
 * @brief Inertia's iterator classes.
 *
 * This header contains Inertia's iterator classes.
 *
 **/

#include <iterator>

namespace inr{

    /**
     * @brief Inertia's vector/array iterator class.
     *
     * This is a super simple iterator for a pointer.
     * You just provide begin() and end() as a pointer and it'll work.
     *
     * Can act as both const and normal iterator if via the templates.
     * So like <const int> or just <int>.
     *
     */
    template<typename T>
    struct array_iterator{
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::random_access_iterator_tag;

        pointer ptr;

        constexpr array_iterator(pointer p) : ptr(p){};

        constexpr reference operator*() const noexcept{ 
            return *ptr;
        }
        constexpr pointer operator->() const noexcept{
            return ptr;
        }

        constexpr array_iterator& operator++() noexcept{
            ptr++;
            return *this;
        }

        constexpr array_iterator& operator++(int) noexcept{
            array_iterator tmp = *this;
            ptr++;
            return tmp;
        }

        constexpr array_iterator& operator--() noexcept{
            ptr--;
            return *this;
        }

        constexpr array_iterator operator--(int) noexcept{
            array_iterator tmp = *this;
            ptr--;
            return tmp;
        }

        constexpr array_iterator operator+(difference_type n) const noexcept{
            return array_iterator(ptr + n);
        }

        constexpr array_iterator operator-(difference_type n) const noexcept{ 
            return array_iterator(ptr - n);
        }

        constexpr difference_type operator-(const array_iterator& other) const noexcept{
            return ptr - other.ptr;
        }

        constexpr array_iterator operator+=(difference_type n) noexcept{
            ptr += n;
            return *this;
        }

        constexpr array_iterator operator-=(difference_type n) noexcept{
            ptr -= n;
            return *this;
        }

        constexpr reference operator[](difference_type n) const noexcept{
            return ptr[n];
        }

        constexpr auto operator<=>(const array_iterator& other) const noexcept{
            return ptr <=> other.ptr;
        }

        constexpr bool operator!=(const array_iterator& other) const noexcept{
            return ptr != other.ptr;
        }
    };

}

#endif // INERTIA_INRITERATOR_HPP
