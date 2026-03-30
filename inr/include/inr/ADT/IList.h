// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_ADT_ILIST_H
#define INERTIA_ADT_ILIST_H

/// @file ADT/IList.h
/// @brief Provides the intrusive list class.

#include <cstddef>

namespace inr {

template<typename T>
class ilist;

/// @brief Intrusive linked list's node.
template<typename T>
class ilist_node {
    /// @brief Previous node.
    T* prev_ = nullptr;
    /// @brief Next node.
    T* next_ = nullptr;

public:
    ilist_node() noexcept = default;

    T* getNext() noexcept {
        return next_;
    }

    T* getPrev() noexcept {
        return prev_;
    }

    const T* getNext() const noexcept {
        return next_;
    }

    const T* getPrev() const noexcept {
        return prev_;
    }

    friend class ilist<T>;
};

/// @brief A simple intrusive linked list implementation.
template<typename T>
class ilist {
public:
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

private:
    /// @brief Head of the list.
    pointer head_ = nullptr;
    /// @brief Tail of the list.
    pointer tail_ = nullptr;

public:
    /// @brief Default list constructor.
    ilist() noexcept = default;

    ilist(const ilist&) = delete;
    ilist& operator=(const ilist&) = delete;

    /// @brief Move constructor.
    ilist(ilist&& other) noexcept : head_(other.head_), tail_(other.tail_) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
    }

    /// @brief Move operator.
    ilist& operator=(ilist&& other) noexcept {
        if(this != &other) {
            head_ = other.head_;
            tail_ = other.tail_;
            other.head_ = nullptr;
            other.tail_ = nullptr;
        }
        return *this;
    }

    /// @brief Pushes a node to the back of the list.
    pointer push_back(pointer node) noexcept {
        node->prev_ = tail_;
        node->next_ = nullptr;
        if(tail_) tail_->next_ = node;
        else head_ = node;
        tail_ = node;
        return node;
    }

    /// @brief Pushes a node to the front of the list.
    pointer push_front(pointer node) noexcept {
        node->next_ = head_;
        node->prev_ = nullptr;
        if(head_) head_->prev_ = node;
        else tail_ = node;
        head_ = node;
        return node;
    }

    /// @brief IList's forward iterator.
    struct iterator {
        pointer current;

        iterator(pointer c) noexcept : current(c) {}

        iterator& operator++() noexcept {
            current = current->next_;
            return *this;
        }

        bool operator!=(const iterator& other) const noexcept {
            return current != other.current;
        }

        reference operator*() noexcept {
            return *current;
        }
    };

    iterator begin() noexcept {
        return iterator(head_);
    }
    iterator end() noexcept {
        return iterator(nullptr);
    }

    iterator begin() const noexcept {
        return iterator(head_);
    }
    iterator end() const noexcept {
        return iterator(nullptr);
    }

    /// @brief Returns the head of the list.
    pointer front() noexcept {
        return head_;
    }

    /// @brief Returns the tail of the list.
    pointer back() noexcept {
        return tail_;
    }

    /// @brief Returns a const head of the list.
    const_pointer front() const noexcept {
        return head_;
    }

    /// @brief Returns a const tail of the list.
    const_pointer back() const noexcept {
        return tail_;
    }

    /// @brief Iterates the list and gives the pointer at that index.
    pointer operator[](size_t n) noexcept {
        iterator it = begin();
        while(n--) {
            ++it;
        }
        return it.current;
    }

    /// @brief Iterates the list and gives the pointer at that index, const
    /// version.
    const_pointer operator[](size_type n) const noexcept {
        iterator it = begin();
        while(n--) {
            ++it;
        }
        return it.current;
    }

    size_type size() const noexcept {
        size_type finalSize = 0;
        for(auto it = begin(); it != end(); ++it, ++finalSize);
        return finalSize;
    }
};

} // namespace inr

#endif // INERTIA_ADT_ILIST_H
