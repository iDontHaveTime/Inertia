#ifndef INERTIA_ADT_ILIST_H
#define INERTIA_ADT_ILIST_H

/// @file ADT/IList.h
/// @brief Provides the intrusive list class.

namespace inr {

/// @brief Intrusive linked list's node.
template<typename T>
class ilist_node {
public:
    /// @brief Previous node.
    T* prev = nullptr;
    /// @brief Next node.
    T* next = nullptr;

    ilist_node() noexcept = default;
};

/// @brief A simple intrusive linked list implementation.
template<typename T>
class ilist {
public:
    /// @brief Head of the list.
    T* head = nullptr;
    /// @brief Tail of the list.
    T* tail = nullptr;

    ilist() noexcept = default;

    ilist(const ilist&) = delete;
    ilist& operator=(const ilist&) = delete;

    ilist(ilist&& other) noexcept : head(other.head), tail(other.tail) {
        other.head = nullptr;
        other.tail = nullptr;
    }

    ilist& operator=(ilist&& other) noexcept {
        if(this != &other) {
            head = other.head;
            tail = other.tail;
            other.head = nullptr;
            other.tail = nullptr;
        }
        return *this;
    }

    T* push_back(T* node) noexcept {
        node->prev = tail;
        node->next = nullptr;
        if(tail) tail->next = node;
        else head = node;
        tail = node;
        return node;
    }

    T* push_front(T* node) noexcept {
        node->next = head;
        node->prev = nullptr;
        if(head) head->prev = node;
        else tail = node;
        head = node;
        return node;
    }

    /// @brief IList's forward iterator.
    struct iterator {
        T* current;

        iterator(T* c) noexcept : current(c) {}

        iterator& operator++() noexcept {
            current = current->next;
            return *this;
        }

        bool operator!=(const iterator& other) const noexcept {
            return current != other.current;
        }

        T& operator*() noexcept {
            return *current;
        }
    };

    iterator begin() noexcept {
        return iterator(head);
    }
    iterator end() noexcept {
        return iterator(nullptr);
    }

    iterator begin() const noexcept {
        return iterator(head);
    }
    iterator end() const noexcept {
        return iterator(nullptr);
    }
};

} // namespace inr

#endif // INERTIA_ADT_ILIST_H
