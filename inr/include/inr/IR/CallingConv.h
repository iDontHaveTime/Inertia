// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_CALLINGCONV
#define INERTIA_IR_CALLINGCONV

/// @file IR/CallingConv.h
/// @brief Provides an enum for calling conventions.

namespace inr {

enum class CallingConv : unsigned short {
    Default, ///< Default calling convention for the target.
    SystemV, ///< SystemV calling convention.
};

} // namespace inr

#endif // INERTIA_IR_CALLINGCONV
