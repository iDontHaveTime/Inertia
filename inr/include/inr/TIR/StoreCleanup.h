// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_STORECLEANUP_H
#define INERTIA_TIR_STORECLEANUP_H

/// @file TIR/StoreCleanup.h
/// @brief Provides the store cleanup pass for TIR.

#include <inr/TIR/TIRPass.h>

namespace inr {

class StoreCleanup : public TIRPass {
public:
    void run(TIRModule*) override;
};

} // namespace inr

#endif // INERTIA_TIR_STORECLEANUP_H
