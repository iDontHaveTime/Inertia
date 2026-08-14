// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_ADT_HSET_H
#define INERTIA_ADT_HSET_H

/// @file ADT/HSet.h
/// @brief Provides a hash-based set.

#include <inr/ADT/HMapInfo.h>
#include <inr/Support/Assert.h>

#include <cstdlib>
#include <cstring>
#include <memory>
#include <utility>

namespace inr {

/// @brief Provides a linear probing hash set.
/// @note Implementation is really similar to HMap's.
///
/// Something probably should be done about the implementation of this and HMap,
/// as right now its kinda repetitive.
template<typename Key, typename Info = HMapInfo<Key>>
class HSet {
public:
    static_assert(alignof(Key) <= alignof(std::max_align_t),
                  "Alignment must be less or equals to std::max_align_t");

    using size_type = std::size_t;

    constexpr static size_type DEFAULT_ALLOC_SIZE = 0x2000;

private:
    enum class State : unsigned char {
        Empty = 0,
        Taken,
        Deleted,
    };

    Key* entries_;
    State* states_;

    size_type size_, capacity_;

    void clearDestruct() {
        if constexpr(std::is_trivially_destructible_v<Key>) {
            size_ = 0;
            return;
        }

        for(size_type i = 0; i < capacity_; i++) {
            if(states_[i] == State::Taken) {
                entries_[i].~Key();
            }
        }
        size_ = 0;
    }

    void clearState() {
        std::memset(states_, 0, sizeof(State) * capacity_);
    }

    void freeSet() {
        clearDestruct();
        std::free(entries_);
    }

    template<typename LookupKey>
    std::pair<Key*, State*> findInsertSlot(const LookupKey& key) const {
        size_type h = Info::hash(key);
        size_type mask = capacity_ - 1;
        size_type index = h & mask;
        Key* deletedSlot = nullptr;
        State* deletedState = nullptr;

        while(true) {
            State s = states_[index];

            if(s == State::Taken) {
                if(Info::equal(key, entries_[index]))
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

    void rehash() {
        if(!capacity_) [[unlikely]] {
            entries_ = (Key*)std::malloc(sizeof(Key) * DEFAULT_ALLOC_SIZE +
                                         sizeof(State) * DEFAULT_ALLOC_SIZE);
            states_ = (State*)(entries_ + DEFAULT_ALLOC_SIZE);
            size_ = 0;
            capacity_ = DEFAULT_ALLOC_SIZE;
            clearState();
            return;
        }

        Key* oldEntries = entries_;
        State* oldStates = states_;
        size_type oldCapacity = capacity_;

        capacity_ <<= 1;

        entries_ = (Key*)std::malloc(sizeof(Key) * capacity_ +
                                     sizeof(State) * capacity_);
        states_ = (State*)(entries_ + capacity_);
        clearState();

        for(size_type i = 0; i < oldCapacity; i++) {
            if(oldStates[i] == State::Taken) {
                Key& oldE = oldEntries[i];
                auto [e, s] = findInsertSlot(oldE);

                inr_assert(*s != State::Taken,
                           "HSet rehash(): slot shouldn't be taken");

                std::construct_at(e, std::move(oldE));

                if constexpr(!std::is_trivially_destructible_v<Key>) {
                    oldE.~Key();
                }

                *s = State::Taken;
            }
        }

        std::free(oldEntries);
    }

    template<typename LookupKey>
    std::pair<size_type, bool> findEntry(const LookupKey& key) const {
        size_type h = Info::hash(key);
        size_type mask = capacity_ - 1;
        size_type index = h & mask;

        while(true) {
            State s = states_[index];

            if(s == State::Taken) {
                if(Info::equal(key, entries_[index])) return {index, true};
            }
            else if(s == State::Empty) {
                return {0, false};
            }

            index = (index + 1) & mask;
        }
    }

public:
    HSet() :
        entries_((Key*)std::malloc(sizeof(Key) * DEFAULT_ALLOC_SIZE +
                                   sizeof(State) * DEFAULT_ALLOC_SIZE)),
        states_((State*)(entries_ + DEFAULT_ALLOC_SIZE)),
        size_(0),
        capacity_(DEFAULT_ALLOC_SIZE) {
        clearState();
    }

    HSet(const HSet& other) = delete;
    HSet& operator=(const HSet&) = delete;

    /// @brief Move constructor.
    HSet(HSet&& other) noexcept :
        entries_(other.entries_),
        states_(other.states_),
        size_(other.size_),
        capacity_(other.capacity_) {
        other.entries_ = other.states_ = nullptr;
        other.size_ = other.capacity_ = 0;
    }

    /// @brief Move operator.
    HSet& operator=(HSet&& other) noexcept {
        if(this != &other) {
            freeSet();
            entries_ = other.entries_;
            states_ = other.states_;
            size_ = other.size_;
            capacity_ = other.capacity_;

            other.entries_ = other.states_ = nullptr;
            other.size_ = other.capacity_ = 0;
        }
        return *this;
    }

    void clear() {
        clearDestruct();
        clearState();
    }

    std::pair<Key*, bool> try_emplace(const Key& key) {
        if(size_ * 4 >= capacity_ * 3) rehash();
        auto [e, s] = findInsertSlot(key);

        if(*s == State::Taken) {
            return {e, false};
        }
        else {
            std::construct_at(e, key);

            *s = State::Taken;
            size_++;
            return {e, true};
        }
    }

    std::pair<Key*, bool> try_emplace(Key&& key) {
        if(size_ * 4 >= capacity_ * 3) rehash();
        auto [e, s] = findInsertSlot(key);

        if(*s == State::Taken) {
            return {e, false};
        }
        else {
            std::construct_at(e, std::move(key));

            *s = State::Taken;
            size_++;
            return {e, true};
        }
    }

    template<typename LookupKey>
    Key* find(const LookupKey& key) {
        auto [v, f] = findEntry(key);
        if(f) {
            return entries_ + v;
        }
        return nullptr;
    }

    template<typename LookupKey>
    const Key* find(const LookupKey& key) const {
        auto [v, f] = findEntry(key);
        if(f) {
            return entries_ + v;
        }
        return nullptr;
    }

    bool erase(const Key& key) {
        auto [v, f] = findEntry(key);
        if(f) {
            if constexpr(!std::is_trivially_destructible_v<Key>) {
                entries_[v].~Key();
            }

            states_[v] = State::Deleted;
            size_--;
            return true;
        }
        return false;
    }

    size_type size() const {
        return size_;
    }

    size_type capacity() const {
        return capacity_;
    }

    ~HSet() {
        freeSet();
    }
};

} // namespace inr

#endif // INERTIA_ADT_HSET_H
