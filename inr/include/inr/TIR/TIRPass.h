// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TIRPASS_H
#define INERTIA_TIR_TIRPASS_H

/// @file TIR/TIRPass.h
/// @brief Provides a pass manager class.

#include <inr/TIR/TIRModule.h>

namespace inr {

class TIRPass {
public:
    virtual void run(TIRModule*) = 0;
};

class TIRPassManager {
    TIRModule* mod_;

public:
    TIRPassManager(TIRModule* mod) noexcept : mod_(mod) {}

    void run(TIRPass* pass) noexcept {
        return pass->run(mod_);
    }
};

} // namespace inr

#endif // INERTIA_TIR_TIRPASS_H
