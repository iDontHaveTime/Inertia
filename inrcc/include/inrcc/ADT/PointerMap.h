// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_ADT_POINTERMAP_H
#define INRCC_ADT_POINTERMAP_H

/// @file ADT/PointerMap.h
/// @brief Contains a map that only works for pointer to pointer key value.

#include <inr/Support/Align.h>

#include <cstdint>
#include <cstdlib>
#include <type_traits>

namespace inrcc {

/// @brief A map that uses a pointer key and value.
/// @note Assumes nullptr is no entry, so nullptr keys are not allowed.
template<typename Key, typename Val>
class PtrMap {
    struct Entry {
        Key key_;
        Val val_;
    };

    Entry* entries_;
    uint32_t size_;
    uint32_t count_;

    constexpr static inline uintptr_t hash(Key k) {
        if constexpr(sizeof(void*) <= 4) {
            return uintptr_t(k) >> 1;
        }
        else {
            return uintptr_t(k) >> 3;
        }
    }

public:
    static_assert(std::is_pointer_v<Key>, "Key must be a pointer.");
    static_assert(std::is_pointer_v<Val>, "Val must be a pointer.");

    PtrMap(inr::Alignment initSize = 0x1000) :
        size_(initSize.getAlignment()), count_(0) {
        entries_ = (Entry*)std::calloc(size_, sizeof(Entry));
    }

    PtrMap(const PtrMap&) = delete;
    PtrMap& operator=(const PtrMap&) = delete;

    /// @brief Move constructor.
    PtrMap(PtrMap&& other) noexcept :
        entries_(other.entries_), size_(other.size_), count_(other.count_) {
        other.entries_ = nullptr;
        other.size_ = other.count_ = 0;
    }

    /// @brief Move operator.
    PtrMap& operator=(PtrMap&& other) noexcept {
        if(this != &other) {
            std::free(entries_);

            entries_ = other.entries_;
            size_ = other.size_;
            count_ = other.count_;

            other.entries_ = nullptr;
            other.size_ = other.count_ = 0;
        }
        return *this;
    }

    ~PtrMap() noexcept {
        std::free(entries_);
    }

    bool insert(Key key, Val val) {
        if(count_ >= (size_ - (size_ >> 2))) {
            rehash();
        }

        uintptr_t index = hash(key) & (size_ - 1);

        while(entries_[index].key_) {
            Entry* entry = entries_ + index;

            if(entry->key_ == key) {
                entry->val_ = val;
                return false;
            }

            index = (index + 1) & (size_ - 1);
        }

        Entry* entry = entries_ + index;
        *entry = {key, val};
        count_++;
        return true;
    }

    size_t size() const noexcept {
        return count_;
    }

    size_t capacity() const noexcept {
        return size_;
    }

    Val find(Key key) const noexcept {
        uintptr_t index = hash(key) & (size_ - 1);
        while(entries_[index].key_) {
            Entry* entry = entries_ + index;

            if(entry->key_ == key) {
                return entry->val_;
            }

            index = (index + 1) & (size_ - 1);
        }

        return nullptr;
    }

private:
    void rehash() noexcept {
        uint32_t oldSize = size_;
        Entry* oldEntries = entries_;

        size_ <<= 1;
        entries_ = (Entry*)std::calloc(size_, sizeof(Entry));

        for(uint32_t i = 0; i < oldSize; i++) {
            if(Entry* entry = oldEntries + i; entry->key_) {
                uintptr_t index = hash(entry->key_) & (size_ - 1);

                while(entries_[index].key_) {
                    index = (index + 1) & (size_ - 1);
                }

                entries_[index] = oldEntries[i];
            }
        }

        std::free(oldEntries);
    }
};

} // namespace inrcc

#endif // INRCC_ADT_POINTERMAP_H
