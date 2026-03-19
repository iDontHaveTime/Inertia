// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_VERSION_H
#define INERTIA_SUPPORT_VERSION_H

/// @file Support/Version.h
/// @brief Provides the version class.

namespace inr {

/// @brief Represents a version with the MAJOR.MINOR.PATCH format.
class Version {
    unsigned major_, minor_, patch_;

public:
    constexpr Version(unsigned major, unsigned minor, unsigned patch) noexcept :
        major_(major), minor_(minor), patch_(patch) {}

    constexpr unsigned getMajor() const noexcept {
        return major_;
    }

    constexpr unsigned getMinor() const noexcept {
        return minor_;
    }

    constexpr unsigned getPatch() const noexcept {
        return patch_;
    }
};

class raw_stream& operator<<(raw_stream&, Version);
Version getInertiaVersion() noexcept;

/// @brief Prints out Inertia's version.
///
/// The format it prints it in is this:
/// Inertia version maj.min.pat
/// @note This does not add a new line.
raw_stream& reportInertiaVersion(raw_stream&);

} // namespace inr

#endif // INERTIA_SUPPORT_VERSION_H
