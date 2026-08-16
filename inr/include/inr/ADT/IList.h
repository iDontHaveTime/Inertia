// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_ADT_ILIST_H
#define INERTIA_ADT_ILIST_H

/// @file ADT/IList.h
/// @brief Provides an intrusive linked list.

#include <inr/Support/Assert.h>

#include <iterator>
#include <type_traits>

namespace inr {

template<typename>
class ilist;

/// @brief An intrusive linked list node.
/// @see `ilist` for more info.
template<typename T>
class ilist_node {
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    pointer next_{};
    pointer prev_{};

    friend class ilist<T>;

public:
    ilist_node() = default;

    ilist_node(const ilist_node&) = default;
    ilist_node& operator=(const ilist_node&) = default;

    ilist_node(ilist_node&&) noexcept = default;
    ilist_node& operator=(ilist_node&&) noexcept = default;

    ~ilist_node() = default;

    pointer getNext() {
        return next_;
    }

    const_pointer getNext() const {
        return next_;
    }

    pointer getPrev() {
        return prev_;
    }

    const_pointer getPrev() const {
        return prev_;
    }
};

template<typename ValT, typename NodeT>
class ilist_iterator {
    NodeT* node_{};

    friend class ilist<std::remove_const_t<ValT>>;

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = ValT;
    using difference_type = std::ptrdiff_t;
    using pointer = ValT*;
    using reference = ValT&;

    ilist_iterator() = default;
    ilist_iterator(NodeT* node) : node_(node) {}

    template<typename V, typename N>
    ilist_iterator(const ilist_iterator<V, N>& other) : node_(other.node_) {}

    reference operator*() const {
        return *(pointer)node_;
    }

    pointer operator->() const {
        return (pointer)node_;
    }

    ilist_iterator& operator++() {
        inr_assert(node_, "ilist_iterator operator++(): past the end");
        node_ = node_->getNext();
        return *this;
    }

    ilist_iterator operator++(int) {
        ilist_iterator tmp(*this);
        ++(*this);
        return tmp;
    }

    ilist_iterator& operator--() {
        inr_assert(node_, "ilist_iterator operator--(): past the begin");
        node_ = node_->getPrev();
        return *this;
    }

    ilist_iterator operator--(int) {
        ilist_iterator tmp(*this);
        --(*this);
        return tmp;
    }

    bool operator==(const ilist_iterator& other) const {
        return node_ == other.node_;
    }

    bool operator!=(const ilist_iterator& other) const {
        return node_ != other.node_;
    }
};

/// @brief Intrusive linked list.
///
/// To create a new intrusive linked list class you would derive it from the
/// node like this:
/// ```cpp
/// class Base : public ilist_node<Base> {...};
/// ```
/// This linked list is not copyable, but is movable.
template<typename T>
class ilist {
public:
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    using iterator = ilist_iterator<value_type, value_type>;
    using const_iterator = ilist_iterator<const value_type, const value_type>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

protected:
    mutable ilist_node<T> sentinel_;

    void initSentinel() noexcept {
        sentinel_.next_ = (pointer)&sentinel_;
        sentinel_.prev_ = (pointer)&sentinel_;
    }

    void unlinkAll() noexcept {
        pointer current = sentinel_.next_;
        while(current != (pointer)&sentinel_) {
            pointer nextNode = current->next_;
            current->next_ = current->prev_ = nullptr;
            current = nextNode;
        }
        initSentinel();
    }

public:
    ilist() {
        initSentinel();
    }

    ilist(const ilist&) = delete;
    ilist& operator=(const ilist&) = delete;

    ilist(ilist&& other) noexcept {
        if(other.empty()) {
            initSentinel();
        }
        else {
            sentinel_.next_ = other.sentinel_.next_;
            sentinel_.prev_ = other.sentinel_.prev_;
            sentinel_.next_->prev_ = (pointer)&sentinel_;
            sentinel_.prev_->next_ = (pointer)&sentinel_;
            other.initSentinel();
        }
    }
    ilist& operator=(ilist&& other) noexcept {
        if(this != &other) {
            unlinkAll();
            if(!other.empty()) {
                sentinel_.next_ = other.sentinel_.next_;
                sentinel_.prev_ = other.sentinel_.prev_;
                sentinel_.next_->prev_ = (pointer)&sentinel_;
                sentinel_.prev_->next_ = (pointer)&sentinel_;
                other.initSentinel();
            }
        }
        return *this;
    }

    ~ilist() = default;

    bool empty() const {
        return sentinel_.next_ == (const_pointer)&sentinel_;
    }

    void clear() {
        unlinkAll();
    }

    pointer push_front(pointer node) {
        inr_assert(
            node && !node->next_ && !node->prev_ && (pointer)&sentinel_ != node,
            "ilist push_front(): used incorrectly");

        node->next_ = sentinel_.next_;
        node->prev_ = (pointer)&sentinel_;
        sentinel_.next_->prev_ = node;
        sentinel_.next_ = node;
        return node;
    }

    pointer push_back(pointer node) {
        inr_assert(
            node && !node->next_ && !node->prev_ && (pointer)&sentinel_ != node,
            "ilist push_back(): used incorrectly");

        node->next_ = (pointer)&sentinel_;
        node->prev_ = sentinel_.prev_;
        sentinel_.prev_->next_ = node;
        sentinel_.prev_ = node;
        return node;
    }

    pointer erase(pointer node) {
        inr_assert(node && node != (pointer)&sentinel_,
                   "ilist erase(): node must be valid");

        node->prev_->next_ = node->next_;
        node->next_->prev_ = node->prev_;
        node->next_ = node->prev_ = nullptr;
        return node;
    }

    pointer listHead() {
        return empty() ? nullptr : sentinel_.next_;
    }

    const_pointer listHead() const {
        return empty() ? nullptr : sentinel_.next_;
    }

    pointer listTail() {
        return empty() ? nullptr : sentinel_.prev_;
    }

    const_pointer listTail() const {
        return empty() ? nullptr : sentinel_.prev_;
    }

    iterator begin() {
        return iterator(sentinel_.next_);
    }

    iterator end() {
        return iterator((pointer)&sentinel_);
    }

    const_iterator begin() const {
        return const_iterator(sentinel_.next_);
    }

    const_iterator end() const {
        return const_iterator((const_pointer)&sentinel_);
    }

    const_iterator cbegin() const {
        return begin();
    }

    const_iterator cend() const {
        return end();
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const {
        return rbegin();
    }

    const_reverse_iterator crend() const {
        return rend();
    }

    iterator erase(iterator it) {
        pointer node = it.node_;
        if(node == (pointer)&sentinel_) return end();

        pointer nextNode = node->next_;
        erase(node);
        return iterator(nextNode);
    }

    void deleteNodes() noexcept {
        pointer current = sentinel_.next_;
        while(current != (pointer)&sentinel_) {
            pointer nextNode = current->next_;
            delete current;
            current = nextNode;
        }
    }

    reference front() {
        pointer h = listHead();
        inr_assert(h != nullptr, "ilist front(): ilist is empty");
        return *h;
    }

    const_reference front() const {
        const_pointer h = listHead();
        inr_assert(h != nullptr, "ilist front() (const): ilist is empty");
        return *h;
    }

    reference back() {
        pointer h = listTail();
        inr_assert(h != nullptr, "ilist back(): ilist is empty");
        return *h;
    }

    const_reference back() const {
        const_pointer h = listTail();
        inr_assert(h != nullptr, "ilist back() (const): ilist is empty");
        return *h;
    }
};

} // namespace inr

#endif // INERTIA_ADT_ILIST_H
