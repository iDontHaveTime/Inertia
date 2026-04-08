// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TIRISEL_H
#define INERTIA_TIR_TIRISEL_H

/// @file TIR/TIRISel.h
/// @brief Provides a class to lower a TIR module to a machine module.

#include <inr/TIR/TIRLowering.h>

namespace inr {

class TIRISel {
    const TIRLowering& usedLowering_;

public:
    TIRISel(const TIRLowering& usedLowering) noexcept :
        usedLowering_(usedLowering) {}

    std::unique_ptr<MachineModule> select(const TIRModule* mod);
};

} // namespace inr

#endif // INERTIA_TIR_TIRISEL_H
