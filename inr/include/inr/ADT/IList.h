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

    /// @brief Returns the next node.
    T* getNext() noexcept {
        return next_;
    }

    /// @brief Returns the previous node.
    T* getPrev() noexcept {
        return prev_;
    }

    /// @brief Returns the next node, const.
    const T* getNext() const noexcept {
        return next_;
    }

    /// @brief Returns the previous node, const.
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
    template<typename ptrT, typename refT>
    struct ilist_iterator {
        ptrT current;

        ilist_iterator(ptrT c) noexcept : current(c) {}

        ilist_iterator& operator++() noexcept {
            if(current) current = current->next_;
            return *this;
        }

        bool operator!=(const ilist_iterator& other) const noexcept {
            return current != other.current;
        }

        refT operator*() noexcept {
            return *current;
        }
    };

    using iterator = ilist_iterator<pointer, reference>;
    using const_iterator = ilist_iterator<const_pointer, const_reference>;

    iterator begin() noexcept {
        return iterator(head_);
    }
    iterator end() noexcept {
        return iterator(nullptr);
    }

    const_iterator begin() const noexcept {
        return const_iterator(head_);
    }
    const_iterator end() const noexcept {
        return const_iterator(nullptr);
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
    pointer operator[](size_type n) noexcept {
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

    /// @brief Calculates the size of the list.
    size_type size() const noexcept {
        size_type finalSize = 0;
        for(auto it = begin(); it != end(); ++it, ++finalSize);
        return finalSize;
    }

    /// @brief Frees all the nodes using `delete`.
    void freeUsingDelete() noexcept {
        pointer i = head_;

        head_ = nullptr;
        tail_ = nullptr;

        while(i) {
            pointer next = i->next_;
            delete i;
            i = next;
        }
    }

    void detach(pointer node) noexcept {
        if(node->prev_) node->prev_->next_ = node->next_;
        else if(head_ == node) head_ = node->next_;

        if(node->next_) node->next_->prev_ = node->prev_;
        else if(tail_ == node) tail_ = node->prev_;

        node->prev_ = nullptr;
        node->next_ = nullptr;
    }

    pointer insertBefore(pointer pos, pointer node) noexcept {
        if(!pos || !node) return node;

        detach(node);

        node->next_ = pos;
        node->prev_ = pos->prev_;

        if(pos->prev_) {
            pos->prev_->next_ = node;
        }
        else {
            head_ = node;
        }

        pos->prev_ = node;
        return node;
    }

    pointer insertAfter(pointer pos, pointer node) noexcept {
        if(!pos || !node) return node;

        detach(node);

        node->prev_ = pos;
        node->next_ = pos->next_;

        if(pos->next_) {
            pos->next_->prev_ = node;
        }
        else {
            tail_ = node;
        }

        pos->next_ = node;
        return node;
    }

    iterator erase(iterator pos) noexcept {
        pointer node = pos.current;
        pointer next = node ? node->next_ : nullptr;

        detach(node);

        return iterator(next);
    }

    iterator erase_delete(iterator it) noexcept {
        pointer node = it.current;
        if(!node) return end();

        pointer next = node->next_;

        detach(node);
        delete node;

        return iterator(next);
    }
};

} // namespace inr

#endif // INERTIA_ADT_ILIST_H
