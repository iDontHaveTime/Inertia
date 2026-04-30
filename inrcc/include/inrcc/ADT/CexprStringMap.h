// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_ADT_CEXPRSTRINGMAP_H
#define INRCC_ADT_CEXPRSTRINGMAP_H

/// @file ADT/CexprStringMap.h
/// @brief Provides a constant expression hash map.

#include <inrcc/ADT/StringMap.h>

namespace inrcc {

namespace internal {

/// @brief Used to represent a string literal in a template.
template<size_t N>
struct cexprstrbase {
    constexpr cexprstrbase(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }
    char value[N];

    constexpr size_t size() const noexcept {
        return N - 1;
    }

    constexpr const char* data() const noexcept {
        return value;
    }
};

/// @brief A key-value struct for constexpr string map.
template<cexprstrbase str, typename T, T val>
struct cexprstrKey {
    constexpr const char* data() const noexcept {
        return str.data();
    }

    constexpr uint32_t size() const noexcept {
        return str.size();
    }

    constexpr T getVal() const noexcept {
        return val;
    }
};

} // namespace internal

/// @brief An alias for the internal struct.
template<internal::cexprstrbase str, typename T, T val>
using CexprStringKey = internal::cexprstrKey<str, T, val>;

/// @brief Hash map with the key (string) being pre-initialized.
///
/// This mimics the behavior of the normal runtime StringMap, thus it is able to
/// provide a fast toRuntime() function. It uses the same hash, rehash, and
/// insert functions, same finding logic as well.
template<typename T, internal::cexprstrKey... Keys>
class CexprStringMap {
    using Entry = StringMap<T>::Entry;

    /// @brief This mimics how StringMap's constructor would work.
    consteval static size_t calculateEntrySize() {
        uint32_t size_ = 16;
        Entry* entries_ = new Entry[size_]();
        uint32_t count_ = 0;

        auto rehash = [&]() {
            uint32_t oldSize = size_;
            Entry* oldEntries = entries_;

            size_ <<= 1;
            entries_ = new Entry[size_](); // cexpr doesn't support calloc

            for(uint32_t i = 0; i < oldSize; i++) {
                if(Entry* entry = oldEntries + i; entry->key_) {
                    uint32_t index = entry->hash_ & (size_ - 1);

                    while(entries_[index].key_) {
                        index = (index + 1) & (size_ - 1);
                    }

                    entries_[index] = oldEntries[i];
                }
            }

            delete[] oldEntries;
        };

        auto insert = [&](const char* key, uint32_t keyLen, T val) {
            if(count_ >= (size_ - (size_ >> 2))) {
                rehash();
            }

            uint32_t hash = StringMap<T>::hashKey(
                key, key + keyLen); // reuse hashing function
            uint32_t index = hash & (size_ - 1);

            while(entries_[index].key_) {
                Entry* entry = entries_ + index;

                if(entry->hash_ == hash) {
                    if(entry->keyLen_ == keyLen) {
                        if(inr::str::compare(entry->key_, key, keyLen) == 0)
                            break;
                    }
                }

                index = (index + 1) & (size_ - 1);
            }

            Entry* entry = entries_ + index;
            *entry = {key, keyLen, hash, val};
            count_++;
        };

        ((insert(Keys.data(), Keys.size(), Keys.getVal())), ...);
        delete[] entries_;

        return size_;
    }
    constexpr static size_t entrySize = calculateEntrySize();

    Entry entries_[entrySize]{};
    uint32_t size_ = entrySize, count_ = sizeof...(Keys);

public:
    static_assert(std::is_trivially_destructible_v<T>,
                  "CexprStringMap's T should be trivially destructible.");
    static_assert(std::is_trivially_constructible_v<T>,
                  "CexprStringMap's T should be trivially constructible.");
    static_assert(std::is_trivially_copyable_v<T>,
                  "CexprStringMap's T should be trivially copyable.");

    /// @brief The only constructor of the cexpr map.
    constexpr CexprStringMap() noexcept {
        // Insert elements, size + count is already known.
        (
            [&] {
                uint32_t hash = StringMap<T>::hashKey(
                    Keys.data(), Keys.data() + Keys.size());
                uint32_t index = hash & (size_ - 1);

                while(entries_[index].key_) {
                    Entry* entry = entries_ + index;

                    if(entry->hash_ == hash) {
                        if(entry->keyLen_ == Keys.size()) {
                            if(inr::str::compare(entry->key_, Keys.data(),
                                                 Keys.size()) == 0)
                                break;
                        }
                    }

                    index = (index + 1) & (size_ - 1);
                }

                entries_[index] = {Keys.data(), Keys.size(), hash,
                                   Keys.getVal()};
            }(),
            ...);
    }

    CexprStringMap(const CexprStringMap&) noexcept = default;
    CexprStringMap& operator=(const CexprStringMap&) noexcept = default;

    CexprStringMap(CexprStringMap&&) noexcept = default;
    CexprStringMap& operator=(CexprStringMap&&) noexcept = default;

    /// @brief Returns how many entries exist.
    constexpr size_t size() const noexcept {
        return count_;
    }

    /// @brief Returns how much entries are possible.
    constexpr size_t capacity() const noexcept {
        return size_;
    }

    /// @brief Tries to find an entry with the same key, key-keylen ver.
    constexpr const T* find(const char* key, uint32_t keyLen) const noexcept {
        uint32_t hash = StringMap<T>::hashKey(key, key + keyLen);
        uint32_t index = hash & (size_ - 1);

        while(entries_[index].key_) {
            const Entry* entry = entries_ + index;
            if(entry->hash_ == hash && entry->keyLen_ == keyLen &&
               inr::str::compare(entry->key_, key, keyLen) == 0) {
                return &entry->value_;
            }
            index = (index + 1) & (size_ - 1);
        }

        return nullptr;
    }

    /// @brief Tries to find an entry with the same key, sview ver.
    constexpr const T* find(inr::sview str) const noexcept {
        return find(str.data(), str.size());
    }

    /// @brief Converts this compile-time map to runtime.
    StringMap<T> toRuntime() const {
        return StringMap<T>((const void*)entries_, size_, count_);
    }

    /// @brief Provides a way to convert a type of this map to runtime.
    ///
    /// Meaning the usage of `using` with this type is preferred.
    /// So if you had:
    /// `using C11_Keywords = CexprStringMap<...>;`
    /// You could just do `C11_Keywords::toRuntimeT();`
    /// The T was added to first of all make it differ from the object version,
    /// and second that it was meant to be used in type situations.
    static StringMap<T> toRuntimeT() {
        constexpr CexprStringMap<T, Keys...> map;
        return map.toRuntime();
    }
};

} // namespace inrcc

#endif // INRCC_ADT_CEXPRSTRINGMAP_H
