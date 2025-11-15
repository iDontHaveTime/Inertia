#ifndef INERTIA_ITERATOR_HPP
#define INERTIA_ITERATOR_HPP

/**
 * @file inr/Support/Iterator.hpp
 * @brief Inertia's iterator classes.
 *
 * This header contains Inertia's iterator classes.
 *
 **/

#include "inr/Defines/CommonTypes.hpp"

#include <iterator>

namespace inr{

    /**
     * @brief Inertia's vector/array iterator class.
     *
     * This is a super simple iterator for a pointer.
     * You just provide begin() and end() (do not use this class) as a pointer and it'll work.
     *
     * Can act as both const and normal iterator if via the templates.
     * So like <const int> or just <int>.
     *
     * This version is the version without ++ or --, its a non-directional version.
     * There are iterators derived from this that ARE directional.
     *
     */
    template<typename T>
    struct fb_array_iterator{
        using value_type = T;
        using difference_type = array_access;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::random_access_iterator_tag;
        pointer ptr;

        constexpr fb_array_iterator(pointer p) noexcept : ptr(p){};
        constexpr ~fb_array_iterator() noexcept = default;

        constexpr reference operator*() const noexcept{ 
            return *ptr;
        }
        constexpr pointer operator->() const noexcept{
            return ptr;
        }

        constexpr auto operator<=>(const fb_array_iterator& other) const noexcept{
            return ptr <=> other.ptr;
        }

        constexpr bool operator!=(const fb_array_iterator& other) const noexcept{
            return ptr != other.ptr;
        }
    };

    /**
     * @brief Inertia's forward array/vector iterator class.
     */
    template<typename T>
    struct array_iterator : public fb_array_iterator<T>{
        using typename fb_array_iterator<T>::value_type;
        using typename fb_array_iterator<T>::difference_type;
        using typename fb_array_iterator<T>::pointer;
        using typename fb_array_iterator<T>::reference;
        using typename fb_array_iterator<T>::iterator_category;

        using fb_array_iterator<T>::fb_array_iterator;

        constexpr array_iterator& operator++() noexcept{
            this->ptr++;
            return *this;
        }

        constexpr array_iterator& operator++(int) noexcept{
            array_iterator tmp = *this;
            this->ptr++;
            return tmp;
        }

        constexpr array_iterator& operator--() noexcept{
            this->ptr--;
            return *this;
        }

        constexpr array_iterator operator--(int) noexcept{
            array_iterator tmp = *this;
            this->ptr--;
            return tmp;
        }

        constexpr array_iterator operator+(difference_type n) const noexcept{
            return array_iterator(this->ptr + n);
        }

        constexpr array_iterator operator-(difference_type n) const noexcept{ 
            return array_iterator(this->ptr - n);
        }

        constexpr difference_type operator-(const array_iterator& other) const noexcept{
            return this->ptr - other.ptr;
        }

        constexpr array_iterator operator+=(difference_type n) noexcept{
            this->ptr += n;
            return *this;
        }

        constexpr array_iterator operator-=(difference_type n) noexcept{
            this->ptr -= n;
            return *this;
        }

        constexpr reference operator[](difference_type n) const noexcept{
            return this->ptr[n];
        }
    };

    /**
     * @brief Inertia's reverse array/vector iterator class.
     */
    template<typename T>
    struct rarray_iterator : public fb_array_iterator<T>{
        using typename fb_array_iterator<T>::value_type;
        using typename fb_array_iterator<T>::difference_type;
        using typename fb_array_iterator<T>::pointer;
        using typename fb_array_iterator<T>::reference;
        using typename fb_array_iterator<T>::iterator_category;

        using fb_array_iterator<T>::fb_array_iterator;

        constexpr rarray_iterator& operator++() noexcept{
            this->ptr--;
            return *this;
        }

        constexpr rarray_iterator& operator++(int) noexcept{
            rarray_iterator tmp = *this;
            this->ptr--;
            return tmp;
        }

        constexpr rarray_iterator& operator--() noexcept{
            this->ptr++;
            return *this;
        }

        constexpr rarray_iterator operator--(int) noexcept{
            rarray_iterator tmp = *this;
            this->ptr++;
            return tmp;
        }

        constexpr rarray_iterator operator+(difference_type n) const noexcept{
            return rarray_iterator(this->ptr - n);
        }

        constexpr rarray_iterator operator-(difference_type n) const noexcept{ 
            return rarray_iterator(this->ptr + n);
        }

        constexpr difference_type operator-(const rarray_iterator& other) const noexcept{
            return other.ptr - this->ptr;
        }

        constexpr rarray_iterator operator+=(difference_type n) noexcept{
            this->ptr -= n;
            return *this;
        }

        constexpr rarray_iterator operator-=(difference_type n) noexcept{
            this->ptr += n;
            return *this;
        }

        constexpr reference operator[](difference_type n) const noexcept{
            return *(this->ptr - n);
        }
    };
}

#endif // INERTIA_ITERATOR_HPP
