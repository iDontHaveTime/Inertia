// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_ADT_STRINGMAP_H
#define INRCC_ADT_STRINGMAP_H

/// @file ADT/StringMap.h
/// @brief Provides a string map class, an alternative to std::unordered_map.

#include <inr/ADT/StrView.h>
#include <inr/Support/Align.h>
#include <inr/Support/String.h>

#include <cstdlib>
#include <initializer_list>

namespace inrcc {

/// @brief Hash map specifically made for strings as a key.
/// @note This does not assume ownership, nor does it copy the strings.
template<typename T, typename Iterator = const char*,
         bool (*CmpFunc)(Iterator itBegin, Iterator itEnd, Iterator it2Begin,
                         Iterator it2End) = nullptr,
         bool FallBackToCmpFunc = false>
class StringMap {
public:
    /// @brief Represents one entry.
    /// @note Caches the hash.
    struct Entry {
        const char* key_; ///< Original string pointer.
        uint32_t keyLen_, hash_;
        T value_;
    };

private:
    Entry* entries_; ///< Entries pointer.
    uint32_t size_;  ///< How many entries are possible.
    uint32_t count_; ///< How many entries are already inside.

public:
    /// @brief djb2 hash.
    constexpr static uint32_t hashKey(Iterator itBegin,
                                      Iterator itEnd) noexcept {
        uint32_t hash = 5381;
        for(; itBegin != itEnd; ++itBegin) {
            hash = ((hash << 5) + hash) + uint8_t(*itBegin);
        }
        return hash;
    }

    static_assert(std::is_trivially_destructible_v<T>,
                  "StringMap's T should be trivially destructible.");
    static_assert(std::is_trivially_constructible_v<T>,
                  "StringMap's T should be trivially constructible.");
    static_assert(std::is_trivially_copyable_v<T>,
                  "StringMap's T should be trivially copyable.");

    /// @brief Creates a new string map.
    /// @param initSize The initial size of the map.
    ///
    /// Reason for using Alignment and not a normal integer is that Alignment
    /// guarantees power of 2. Also it using uint32_t underneath is a nice
    /// addition, as size_ also uses uint32_t.
    StringMap(inr::Alignment initSize = 0x1000) :
        size_(initSize.getAlignment()), count_(0) {
        entries_ = (Entry*)calloc(size_, sizeof(Entry));
    }

    /// @brief This function should not be used.
    /// @note It is used internally by CexprStringMap.
    StringMap(const void* entries, uint32_t capacity, uint32_t count) {
        entries_ = malloc(capacity * sizeof(Entry));
        size_ = capacity;
        count_ = count;
        std::memcpy(entries_, entries, capacity * sizeof(Entry));
    }

    /// @brief Creates a new map and initializes entries with the list.
    ///
    /// Ideally prefer CexprStringMap::toRuntime() since it will pre-compute
    /// entries, size, etc..
    StringMap(std::initializer_list<std::pair<inr::sview, T>> initList,
              inr::Alignment initSize = 0x1000) :
        StringMap(initSize) {
        for(auto it = initList.begin(); it != initList.end(); ++it) {
            insert(it->first, it->second);
        }
    }

    StringMap(const StringMap&) = delete;
    StringMap& operator=(const StringMap&) = delete;

    /// @brief Move constructor.
    StringMap(StringMap&& other) noexcept :
        entries_(other.entries_), size_(other.size_), count_(other.count_) {
        other.entries_ = nullptr;
        other.size_ = other.count_ = 0;
    }

    /// @brief Move operator.
    StringMap& operator=(StringMap&& other) noexcept {
        if(this != &other) {
            free(entries_);

            entries_ = other.entries_;
            size_ = other.size_;
            count_ = other.count_;

            other.entries_ = nullptr;
            other.size_ = other.count_ = 0;
        }
        return *this;
    }

    ~StringMap() noexcept {
        free(entries_);
    }

private:
    const Entry* findEntry(const char* key, uint32_t keyLen) const noexcept {
        uint32_t hash = hashKey(Iterator(key), Iterator(key + keyLen));
        uint32_t index = hash & (size_ - 1);

        while(entries_[index].key_) {
            const Entry* entry = entries_ + index;

            if(entry->hash_ == hash) {
                if(entry->keyLen_ == keyLen) {
                    if(inr::str::compare(entry->key_, key, keyLen) == 0)
                        return entry;
                }
                else if constexpr(CmpFunc != nullptr &&
                                  FallBackToCmpFunc == true) {
                    if(CmpFunc(Iterator(entry->key_),
                               Iterator(entry->key_ + entry->keyLen_), key,
                               key + keyLen)) {
                        return entry;
                    }
                }
            }

            index = (index + 1) & (size_ - 1);
        }
        return nullptr;
    }

    Entry* findEntry(const char* key, uint32_t keyLen) noexcept {
        uint32_t hash = hashKey(Iterator(key), Iterator(key + keyLen));
        uint32_t index = hash & (size_ - 1);

        while(entries_[index].key_) {
            Entry* entry = entries_ + index;

            if(entry->hash_ == hash) {
                if(entry->keyLen_ == keyLen) {
                    if(inr::str::compare(entry->key_, key, keyLen) == 0)
                        return entry;
                }
                else if constexpr(CmpFunc != nullptr &&
                                  FallBackToCmpFunc == true) {
                    if(CmpFunc(Iterator(entry->key_),
                               Iterator(entry->key_ + entry->keyLen_), key,
                               key + keyLen)) {
                        return entry;
                    }
                }
            }

            index = (index + 1) & (size_ - 1);
        }
        return nullptr;
    }

    void eraseEntry(Entry* entry) noexcept {
        Entry* end = entries_ + size_;

        while(entry + 1 < end && (entry + 1)->key_) {
            *entry = *(entry + 1);
            entry++;
        }

        entry->key_ = nullptr;
        count_--;
    }

public:
    /// @brief Const version of finding an element, key-keylen ver.
    /// @param key Pointer to the string.
    /// @param keyLen Length of the string.
    const T* find(const char* key, uint32_t keyLen) const noexcept {
        const Entry* e = findEntry(key, keyLen);
        return e ? &e->value_ : nullptr;
    }

    /// @brief Const version of finding an element, sview ver.
    /// @param str String view.
    const T* find(inr::sview str) const noexcept {
        return find(str.data(), str.size());
    }

    /// @brief Finds an entry, key-keylen ver.
    /// @param key Pointer to the string.
    /// @param keyLen Length of the string.
    T* find(const char* key, uint32_t keyLen) noexcept {
        Entry* e = findEntry(key, keyLen);
        return e ? &e->value_ : nullptr;
    }

    /// @brief Finds an entry, sview ver.
    /// @param str String view.
    T* find(inr::sview str) noexcept {
        return find(str.data(), str.size());
    }

    /// @brief Erases the provided key.
    /// @return True if erased.
    bool erase(const char* key, uint32_t keyLen) noexcept {
        uint32_t hash = hashKey(Iterator(key), Iterator(key + keyLen));
        uint32_t index = hash & (size_ - 1);

        Entry* foundEntry = nullptr;
        while(entries_[index].key_) {
            Entry* entry = entries_ + index;

            if(entry->hash_ == hash) {
                if(entry->keyLen_ == keyLen) {
                    if(inr::str::compare(entry->key_, key, keyLen) == 0) {
                        foundEntry = entry;
                        break;
                    }
                }
                else if constexpr(CmpFunc != nullptr &&
                                  FallBackToCmpFunc == true) {
                    if(CmpFunc(Iterator(entry->key_),
                               Iterator(entry->key_ + entry->keyLen_), key,
                               key + keyLen)) {
                        foundEntry = entry;
                        break;
                    }
                }
            }

            index = (index + 1) & (size_ - 1);
        }

        if(foundEntry) {
            eraseEntry(foundEntry);
            // Clean up entries after it?
        }

        return foundEntry != nullptr;
    }

    bool erase(inr::sview str) noexcept {
        return erase(str.data(), str.size());
    }

    bool eraseIfFound(const char* key, uint32_t keyLen) noexcept {
        Entry* e = findEntry(key, keyLen);
        if(e) {
            eraseEntry(e);
            return true;
        }
        return false;
    }

    bool eraseIfFound(inr::sview str) noexcept {
        return eraseIfFound(str.data(), str.size());
    }

    /// @brief Inserts a new value with they key, key-keylen ver.
    T* insert(const char* key, uint32_t keyLen, T val) noexcept {
        if(count_ >= (size_ - (size_ >> 2))) {
            rehash();
        }

        uint32_t hash = hashKey(Iterator(key), Iterator(key + keyLen));
        uint32_t index = hash & (size_ - 1);

        while(entries_[index].key_) {
            Entry* entry = entries_ + index;

            if(entry->hash_ == hash) {
                if(entry->keyLen_ == keyLen) {
                    if(inr::str::compare(entry->key_, key, keyLen) == 0) break;
                }
                else if constexpr(CmpFunc != nullptr &&
                                  FallBackToCmpFunc == true) {
                    if(CmpFunc(Iterator(entry->key_),
                               Iterator(entry->key_ + entry->keyLen_), key,
                               key + keyLen)) {
                        break;
                    }
                }
            }

            index = (index + 1) & (size_ - 1);
        }

        Entry* entry = entries_ + index;
        *entry = {key, keyLen, hash, val};
        count_++;
        return &entry->value_;
    }

    /// @brief Inserts a new value with they key, sview ver.
    T* insert(inr::sview str, T val) noexcept {
        return insert(str.data(), str.size(), val);
    }

    /// @brief Returns how many entries are inside the map.
    size_t size() const noexcept {
        return count_;
    }

    /// @brief Returns how much entries this map has allocated.
    size_t capacity() const noexcept {
        return size_;
    }

private:
    void rehash() noexcept {
        uint32_t oldSize = size_;
        Entry* oldEntries = entries_;

        size_ <<= 1;
        entries_ = (Entry*)calloc(size_, sizeof(Entry));

        for(uint32_t i = 0; i < oldSize; i++) {
            if(Entry* entry = oldEntries + i; entry->key_) {
                uint32_t index = entry->hash_ & (size_ - 1);

                while(entries_[index].key_) {
                    index = (index + 1) & (size_ - 1);
                }

                entries_[index] = oldEntries[i];
            }
        }

        free(oldEntries);
    }
};

} // namespace inrcc

#endif // INRCC_ADT_STRINGMAP_H
