// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_SELECT
#define INERTIA_TIR_SELECT

/// @file TIR/Select.h
/// @brief Selects generic instructions into machine instructions.

#include <inr/TIR/TInst.h>

namespace inr {

class Select {
public:
    virtual ~Select() = default;

    virtual bool select(TInst* inst) = 0;
};

} // namespace inr

#endif // INERTIA_TIR_SELECT
