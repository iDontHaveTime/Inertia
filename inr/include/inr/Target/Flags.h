// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TARGET_FLAGS_H
#define INERTIA_TARGET_FLAGS_H

/// @file Target/Flags.h
/// @brief Provides various flags to be used during compilation.

#include <cstdint>

namespace inr {

class Flags {
    enum RelocModel : uint8_t { Static, Pie, Pic, PIE, PIC };

    RelocModel relocModel_ = Static;
    bool frameReg_ = false;

public:
    Flags() noexcept = default;

    void setRelocModel(RelocModel relocModel) noexcept {
        relocModel_ = relocModel;
    }

    RelocModel getRelocModel() const noexcept {
        return relocModel_;
    }

    void setFrameRegister(bool enabled) noexcept {
        frameReg_ = enabled;
    }

    bool getFrameRegister() const noexcept {
        return frameReg_;
    }
};

} // namespace inr

#endif // INERTIA_TARGET_FLAGS_H
