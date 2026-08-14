// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_ADT_HMAP_H
#define INERTIA_ADT_HMAP_H

/// @file ADT/HMap.h
/// @brief Provides a flexible hash map.

#include <inr/ADT/HMapInfo.h>
#include <inr/Support/Assert.h>

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <type_traits>
#include <utility>

namespace inr {

/// @brief Linear probing hash map.
/// @note Not exception safe.
///
/// This is a linear probing hash map that uses the provided info to store
/// elements. The info should provide:
/// `hash(const Key&)` and `equal(const Key&, const Key&)`.
/// Both `hash` and `equal` should be static and return `std::size_t` and `bool`
/// respectively.
template<typename Key, typename Val, typename Info = HMapInfo<Key>>
class HMap {
public:
    static_assert(alignof(Key) <= alignof(std::max_align_t) &&
                      alignof(Val) <= alignof(std::max_align_t),
                  "Alignment must be less or equals to std::max_align_t");

    using size_type = std::size_t;

    constexpr static size_type DEFAULT_ALLOC_SIZE = 0x2000;

private:
    struct Entry {
        Key key_;
        Val val_;
    };
    enum class State : unsigned char {
        Empty = 0,
        Taken,
        Deleted,
    };
    Entry* entries_;
    State* states_;

    size_type size_, capacity_;

    void clearDestruct() {
        if constexpr(std::is_trivially_destructible_v<Key> &&
                     std::is_trivially_destructible_v<Val>) {
            size_ = 0;
            return;
        }

        for(size_type i = 0; i < capacity_; i++) {
            if(states_[i] == State::Taken) {
                Entry& e = entries_[i];

                if constexpr(!std::is_trivially_destructible_v<Key>) {
                    e.key_.~Key();
                }

                if constexpr(!std::is_trivially_destructible_v<Val>) {
                    e.val_.~Val();
                }
            }
        }
        size_ = 0;
    }

    void clearState() {
        std::memset(states_, 0, sizeof(State) * capacity_);
    }

    void freeMap() {
        clearDestruct();
        std::free(entries_);
    }

    void rehash() {
        if(!capacity_) [[unlikely]] {
            entries_ = (Entry*)std::malloc(sizeof(Entry) * DEFAULT_ALLOC_SIZE +
                                           sizeof(State) * DEFAULT_ALLOC_SIZE);
            states_ = (State*)(entries_ + DEFAULT_ALLOC_SIZE);
            size_ = 0;
            capacity_ = DEFAULT_ALLOC_SIZE;
            clearState();
            return;
        }

        Entry* oldEntries = entries_;
        State* oldStates = states_;
        size_type oldCapacity = capacity_;

        capacity_ <<= 1;

        entries_ = (Entry*)std::malloc(sizeof(Entry) * capacity_ +
                                       sizeof(State) * capacity_);
        states_ = (State*)(entries_ + capacity_);
        clearState();

        for(size_type i = 0; i < oldCapacity; i++) {
            if(oldStates[i] == State::Taken) {
                Entry& oldE = oldEntries[i];
                auto [e, s] = findInsertSlot(oldE.key_);

                inr_assert(*s != State::Taken,
                           "HMap rehash(): slot shouldn't be taken");

                std::construct_at(&e->key_, std::move(oldE.key_));
                std::construct_at(&e->val_, std::move(oldE.val_));

                if constexpr(!std::is_trivially_destructible_v<Key>) {
                    oldE.key_.~Key();
                }

                if constexpr(!std::is_trivially_destructible_v<Val>) {
                    oldE.val_.~Val();
                }

                *s = State::Taken;
            }
        }

        std::free(oldEntries);
    }

    std::pair<size_type, bool> findEntry(const Key& key) const {
        size_type h = Info::hash(key);
        size_type mask = capacity_ - 1;
        size_type index = h & mask;

        while(true) {
            State s = states_[index];

            if(s == State::Taken) {
                if(Info::equal(key, entries_[index].key_)) return {index, true};
            }
            else if(s == State::Empty) {
                return {0, false};
            }

            index = (index + 1) & mask;
        }
    }

    std::pair<Entry*, State*> findInsertSlot(const Key& key) const {
        size_type h = Info::hash(key);
        size_type mask = capacity_ - 1;
        size_type index = h & mask;
        Entry* deletedSlot = nullptr;
        State* deletedState = nullptr;

        while(true) {
            State s = states_[index];

            if(s == State::Taken) {
                if(Info::equal(key, entries_[index].key_))
                    return {entries_ + index, states_ + index};
            }
            else if(s == State::Deleted) {
                if(!deletedSlot) {
                    deletedSlot = entries_ + index;
                    deletedState = states_ + index;
                }
            }
            else {
                if(deletedSlot) {
                    return {deletedSlot, deletedState};
                }
                return {entries_ + index, states_ + index};
            }

            index = (index + 1) & mask;
        }
    }

public:
    /// @brief Constructs a new map with the `DEFAULT_ALLOC_SIZE` amount of
    /// entries.
    HMap() :
        entries_((Entry*)std::malloc(sizeof(Entry) * DEFAULT_ALLOC_SIZE +
                                     sizeof(State) * DEFAULT_ALLOC_SIZE)),
        states_((State*)(entries_ + DEFAULT_ALLOC_SIZE)),
        size_(0),
        capacity_(DEFAULT_ALLOC_SIZE) {
        clearState();
    }

    // Copy constructor/operator deleted for now (or forever?).

    HMap(const HMap& other) = delete;
    HMap& operator=(const HMap&) = delete;

    /// @brief Move constructor.
    HMap(HMap&& other) noexcept :
        entries_(other.entries_),
        states_(other.states_),
        size_(other.size_),
        capacity_(other.capacity_) {
        other.entries_ = other.states_ = nullptr;
        other.size_ = other.capacity_ = 0;
    }

    /// @brief Move operator.
    HMap& operator=(HMap&& other) noexcept {
        if(this != &other) {
            freeMap();
            entries_ = other.entries_;
            states_ = other.states_;
            size_ = other.size_;
            capacity_ = other.capacity_;

            other.entries_ = other.states_ = nullptr;
            other.size_ = other.capacity_ = 0;
        }
        return *this;
    }

    /// @brief Resets the amount of elements.
    /// @note Does not free memory.
    void clear() {
        clearDestruct();
        clearState();
    }

    /// @brief Tries to emplace the value and with the provided key.
    /// @return A pair with the pointer to the value and boolean if emplaced or
    /// not.
    template<typename... Args>
    std::pair<Val*, bool> try_emplace(const Key& key, Args&&... args) {
        if(size_ * 4 >= capacity_ * 3) rehash();
        auto [e, s] = findInsertSlot(key);

        if(*s == State::Taken) {
            return {&e->val_, false};
        }
        else {
            std::construct_at(&e->key_, key);
            std::construct_at(&e->val_, std::forward<Args>(args)...);

            *s = State::Taken;
            size_++;
            return {&e->val_, true};
        }
    }

    /// @brief Tries to emplace the value and move the key.
    /// @note Does not move the key if it didn't emplace.
    /// @return A pair with the pointer to the value and boolean if emplaced or
    /// not.
    template<typename... Args>
    std::pair<Val*, bool> try_emplace(Key&& key, Args&&... args) {
        if(size_ * 4 >= capacity_ * 3) rehash();
        auto [e, s] = findInsertSlot(key);

        if(*s == State::Taken) {
            return {&e->val_, false};
        }
        else {
            std::construct_at(&e->key_, std::move(key));
            std::construct_at(&e->val_, std::forward<Args>(args)...);

            *s = State::Taken;
            size_++;
            return {&e->val_, true};
        }
    }

    /// @brief Finds an element with the provided key.
    /// @return Pointer to the value if found, nullptr if not.
    Val* find(const Key& key) {
        auto [v, f] = findEntry(key);
        if(f) {
            return &entries_[v].val_;
        }
        return nullptr;
    }

    /// @brief Finds an element with the provided key.
    /// @return Pointer to the value if found, nullptr if not.
    const Val* find(const Key& key) const {
        auto [v, f] = findEntry(key);
        if(f) {
            return &entries_[v].val_;
        }
        return nullptr;
    }

    /// @brief Tries to erase an element at the provided key.
    /// @return True if erased, false if not found.
    bool erase(const Key& key) {
        auto [v, f] = findEntry(key);
        if(f) {
            if constexpr(!std::is_trivially_destructible_v<Key>) {
                entries_[v].key_.~Key();
            }

            if constexpr(!std::is_trivially_destructible_v<Val>) {
                entries_[v].val_.~Val();
            }

            states_[v] = State::Deleted;
            size_--;
            return true;
        }
        return false;
    }

    /// @brief Returns the amount of entries in the map.
    size_type size() const {
        return size_;
    }

    /// @brief Returns how many entries can this map hold.
    size_type capacity() const {
        return capacity_;
    }

    ~HMap() {
        freeMap();
    }
};

} // namespace inr

#endif // INERTIA_ADT_HMAP_H
