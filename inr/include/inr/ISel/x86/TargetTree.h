// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_X86_TARGETTREE_H
#define INERTIA_X86_TARGETTREE_H

/// @file ISel/x86/TargetTree.h
/// @brief Contains the x86 target tree get function.

#include <inr/ISel/TargetTree.h>

namespace inr::x86 {

arrview<TreeNodeObjectFunc> getTargetTree();

} // namespace inr::x86

#endif // INERTIA_X86_TARGETTREE_H
