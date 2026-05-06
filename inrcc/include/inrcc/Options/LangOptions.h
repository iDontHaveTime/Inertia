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
    /// @brief This represents a language inrcc might or might not support.
    enum Standard : uint32_t {
        Unknown,
#define INRCC_LANG(IDENT, SUPPORTED, STR, ...) IDENT,
#include <inrcc/Options/LangOptions.inc>
#undef INRCC_LANG
        STANDARD_END
    };

    static bool isLanguageSupported(Standard lang) noexcept;
    static const char* getAsString(Standard lang) noexcept;

private:
    /// @brief Standard of the language.
    Standard standard_ = Unknown;
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
    /// Bit 2 - Extensions: gnu89, gnu99, etc..
    unsigned feats_ = 0;

public:
    constexpr Language() noexcept = default;

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

    constexpr bool getExtensions() const noexcept {
        return feats_ & 0x4;
    }

    constexpr void enableTrigraph() noexcept {
        feats_ |= 0x1;
    }

    constexpr void enableSpaceship() noexcept {
        feats_ |= 0x2;
    }

    constexpr void enableExtensions() noexcept {
        feats_ |= 0x4;
    }

    static Language getFromString(inr::sview str) noexcept;
};

} // namespace inrcc

#endif // INRCC_OPTIONS_LANGOPTIONS_H
