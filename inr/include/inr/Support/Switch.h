// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_SWITCH_H
#define INERTIA_SUPPORT_SWITCH_H

/// @file Support/Switch.h
/// @brief Provides a generic switch class for custom classes.

#include <inr/ADT/StrView.h>

namespace inr {

/// @brief A generic implementation for a switch-like syntax for custom classes.
template<typename Key, typename Val>
class Switch {
    const Key key_;
    Val value_;
    bool caseFound_;

public:
    Switch(const Key& key) noexcept : key_(key) {}

    /// @brief Adds a new case, when the key is equals to the initial key sets
    /// the value.
    Switch& newCase(const Key& key, Val value) noexcept {
        if(!caseFound_ && key == key_) {
            value_ = value;
            caseFound_ = true;
        }
        return *this;
    }

    /// @brief If no case matched, sets the value to this.
    Switch& setDefault(Val value) noexcept {
        if(!caseFound_) {
            value_ = value;
            caseFound_ = true;
        }
        return *this;
    }

    /// @brief Returns the stored value inside.
    operator Val() const noexcept {
        return value_;
    }
};

template<typename Val>
using StrSwitch = Switch<sview, Val>;

} // namespace inr

#endif // INERTIA_SUPPORT_SWITCH_H