// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/ISel/TargetTree.h>
#include <inr/ISel/x86/TargetTree.h>

/// @file ISel/x86/TargetTree.cpp
/// @brief This file contains the x86 instruction tree.
///
/// This file contains the instructions for x86 using the target tree class
/// found inside `inr/ISel/TargetTree.h`.

namespace inr::x86 {

arrview<TreeNodeObjectFunc> getTargetTree() {
    return {};
}

} // namespace inr::x86