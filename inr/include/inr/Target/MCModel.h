// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TARGET_MCMODEL
#define INERTIA_TARGET_MCMODEL

/// @file Target/MCModel.h
/// @brief Used for the `-mcmodel` flag.s

namespace inr {

enum class MCModel : unsigned char {
    Small,
    Medium,
    Large,
};

}

#endif // INERTIA_TARGET_MCMODEL
