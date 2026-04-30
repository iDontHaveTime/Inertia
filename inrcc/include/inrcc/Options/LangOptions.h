// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_OPTIONS_LANGOPTIONS_H
#define INRCC_OPTIONS_LANGOPTIONS_H

/// @file Options/LangOptions.h
/// @brief Represents language options.

#include <inr/ADT/StrView.h>

#include <cstdint>

namespace inrcc {

class Language {
public:
    enum Standard : uint32_t {
/// @brief This represents a language inrcc might or might not support.
#define INRCC_LANG(IDENT, SUPPORTED, STR, VAL) IDENT = VAL,

#include <inrcc/Options/LangOptions.inc>

#undef INRCC_LANG
    };

    static bool isLanguageSupported(Standard lang) noexcept;
    static const char* getAsString(Standard lang) noexcept;

private:
    /// @brief Standard of the language.
    Standard standard_;
    /// @brief Features of the language.
    ///
    /// Bit 0 - Trigraphs:
    /// - ??= #
    /// - ??( [
    /// - ??/ backslash
    /// - ??) ]
    /// - ??' ^
    /// - ??< {
    /// - ??! |
    /// - ??> }
    /// - ??- ~
    /// Bit 1 - Spaceship operator: <=>
    unsigned feats_ = 0;

public:
    constexpr Language() noexcept : standard_(Unknown) {}

    constexpr Language(const Language&) noexcept = default;
    constexpr Language& operator=(const Language&) noexcept = default;

    constexpr Language(Language&&) noexcept = default;
    constexpr Language& operator=(Language&&) noexcept = default;

    constexpr Language(Standard standard) noexcept : standard_(standard) {}

    constexpr Standard getStandard() const noexcept {
        return standard_;
    }

    constexpr void setStandard(Standard standard) noexcept {
        standard_ = standard;
    }

    constexpr inr::sview getAsString() const noexcept {
        return getAsString(standard_);
    }

    constexpr bool isSupported() const noexcept {
        return isLanguageSupported(standard_);
    }

    constexpr bool getTrigraph() const noexcept {
        return feats_ & 0x1;
    }

    constexpr bool getSpaceship() const noexcept {
        return feats_ & 0x2;
    }

    constexpr void enableTrigraph() noexcept {
        feats_ |= 0x1;
    }

    constexpr void enableSpaceship() noexcept {
        feats_ |= 0x2;
    }

    static Language getFromString(inr::sview str) noexcept;
};

} // namespace inrcc

#endif // INRCC_OPTIONS_LANGOPTIONS_H
