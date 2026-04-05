// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_ADT_STRVIEW_H
#define INERTIA_ADT_STRVIEW_H

/// @file ADT/StrView.h
/// @brief Provides a string class similar to std::string_view.

#include <inr/Support/Stream.h>
#include <inr/Support/String.h>

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>

namespace inr {

/// @brief A non-owning string type.
class sview {
public:
    using value_type = char;
    using iterator = const value_type*;
    using const_iterator = iterator;
    using size_type = size_t;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    const char* str_ = nullptr;
    size_t len_ = 0;

public:
    /// @brief Constructs an empty string reference.
    constexpr sview() noexcept = default;

    /// @brief Disable passing in nullptr.
    constexpr sview(std::nullptr_t) = delete;

    /// @brief Creates a reference to the string provided.
    /// @param str C string.
    constexpr sview(const char* str) noexcept :
        str_(str), len_(inr::str::length(str)) {}

    /// @brief Creates a new string reference from start and end pointers.
    constexpr sview(const char* ptr1, const char* ptr2) noexcept :
        str_(ptr1), len_(ptr2 > ptr1 ? ptr2 - ptr1 : 0) {}

    /// @brief Creates a new string reference from the pointer and size
    /// provided.
    /// @param data Start of the string.
    /// @param length Length of the string.
    constexpr sview(const char* data, size_t length) noexcept :
        str_(data), len_(length) {}

    /// @brief Constructs a new string reference from an std::string.
    /// @param str The string to construct from.
    constexpr sview(const std::string& str) noexcept :
        str_(str.data()), len_(str.size()) {}

    /// @brief Constructs a new string reference from an std::string_view.
    /// @param sv String view.
    constexpr sview(std::string_view sv) noexcept :
        str_(sv.data()), len_(sv.size()) {}

    constexpr sview(const sview&) = default;
    constexpr sview& operator=(const sview&) = default;

    constexpr sview(sview&&) = default;
    constexpr sview& operator=(sview&&) = default;

    constexpr iterator begin() const noexcept {
        return str_;
    }

    constexpr iterator end() const noexcept {
        return str_ + len_;
    }

    constexpr reverse_iterator rbegin() const noexcept {
        return reverse_iterator(end());
    }

    constexpr reverse_iterator rend() const noexcept {
        return reverse_iterator(begin());
    }

    /// @brief Gets the string's pointer.
    /// @return Const pointer to the string.
    constexpr const char* data() const noexcept {
        return str_;
    }

    /// @brief Checks if the string is empty or not.
    /// @return True if empty, false if not.
    constexpr bool empty() const noexcept {
        return !len_;
    }

    /// @brief Gets the length of the string.
    constexpr size_t size() const noexcept {
        return len_;
    }

    /// @brief Gets the first character of the string.
    /// @return First character of the string.
    constexpr char front() const noexcept {
        return str_[0];
    }

    /// @brief Gets the last character of the string.
    /// @return Last character of the string.
    constexpr char back() const noexcept {
        return str_[len_ - 1];
    }

    /// @brief Returns this as a heap string.
    /// @return C++ std::string.
    std::string str() const {
        return std::string(str_, len_);
    }

    constexpr std::string_view strv() const noexcept {
        return std::string_view(str_, len_);
    }

    /// @brief Access a certain character in the string.
    /// @param index Index of the character.
    /// @return Character in that index.
    constexpr char operator[](size_t index) const noexcept {
        return str_[index];
    }

    /// @brief Casts this to a string_view.
    constexpr operator std::string_view() const noexcept {
        return std::string_view(str_, len_);
    }

    /// @brief Be able to print this to a stream.
    friend raw_stream& operator<<(raw_stream& os, sview sw) {
        return os.write(sw.data(), sw.size());
    }

    /// @brief Compares a string lexicographically.
    constexpr std::strong_ordering operator<=>(sview other) const {
        return std::lexicographical_compare_three_way(
            str_, str_ + len_, other.str_, other.str_ + other.len_);
    }

    /// @brief Compares two string views.
    constexpr bool operator==(sview other) const {
        return (*this <=> other) == std::strong_ordering::equal;
    }

    /// @brief Finds a character in the string and returns its index.
    /// @param c The character to find.
    /// @param from From what index to start.
    /// @return Index of the character found.
    constexpr size_t find(char c, size_t from = 0) const noexcept {
        if(from >= len_) return len_;

        while(from < len_) {
            if(str_[from] == c) return from;
            from++;
        }

        return from;
    }

    /// @brief Same as find(char, size_t) but case insensitive.
    /// @param c Character to find.
    /// @param from From what index to start.
    /// @return Index of the character.
    constexpr size_t find_insensitive(char c, size_t from = 0) const noexcept {
        if(from >= len_) return len_;

        if(std::isalpha(c)) {
            char c1 = isupper(c) ? tolower(c) : toupper(c);
            while(from < len_) {
                char cmp = str_[from];
                if(cmp == c || cmp == c1) return from;
                from++;
            }
            return from;
        }
        else {
            return find(c, from);
        }
    }

    /// @brief Creates a new substring.
    /// @param start From what character to start.
    /// @param n How many characters.
    /// @return A new string view.
    constexpr sview substr(size_t start, size_t n) const noexcept {
        return sview(str_ + start, n);
    }

    /// @brief Returns the amount of times a character appears.
    /// @param c The character to look for.
    /// @return How many characters were found.
    constexpr size_t count(char c) const noexcept {
        size_t n = 0;
        for(size_t i = 0; i < len_; i++) {
            if(str_[i] == c) n++;
        }
        return n;
    }

    /// @brief Creates a new substring from one index to another.
    /// @param from From what index to start.
    /// @param to What index to end at.
    /// @return A new string view.
    constexpr sview slice(size_t from, size_t to) const noexcept {
        if(from >= to) return sview();
        return sview(str_ + from, str_ + to);
    }

    /// @brief Alias for hashing this.
    size_t hash() const noexcept {
        return std::hash<std::string_view>{}(strv());
    }
};

} // namespace inr

namespace std {

/// @brief Make the `inr::sview` hash same as `std::string_view`.
template<>
struct hash<inr::sview> {
    /// @brief Casts the `inr::sview` to `std::string_view`.
    size_t operator()(inr::sview sv) const noexcept {
        return sv.hash();
    }
};

} // namespace std

#endif // INERTIA_ADT_STRVIEW_H
