// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_VERSION
#define INERTIA_SUPPORT_VERSION

/// @file Support/Version.h
/// @brief Represents a 3 integer version following the Maj.Min.Pat format.

#include <cstdint>

namespace inr {

class Version {
public:
    using version_t = uint16_t;

private:
    version_t major_, minor_, patch_;

public:
    /// @brief Represents a version in the Maj.Min.Pat format.
    /// @param maj Major version, X.y.z
    /// @param min Minor version, x.Y.z
    /// @param pat Patch version, x.y.Z
    constexpr Version(version_t maj, version_t min, version_t pat) :
        major_(maj), minor_(min), patch_(pat) {}

    /// @brief Returns the major version.
    version_t getMajor() const {
        return major_;
    }

    /// @brief Returns the minor version.
    version_t getMinor() const {
        return minor_;
    }

    /// @brief Returns the patch version.
    version_t getPatch() const {
        return patch_;
    }

    friend class stream& operator<<(stream&, Version);
};

Version getInertiaVersion();

} // namespace inr

#endif // INERTIA_SUPPORT_VERSION
