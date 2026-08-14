// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TARGET_HOST_H
#define INERTIA_TARGET_HOST_H

/// @file Target/Host.h
/// @brief Describes the host architecture.

#include <inr/Target/Target.h>

namespace inr::host {

/// @brief Returns the target triple of this host.
Target getTarget();

} // namespace inr::host

#endif // INERTIA_TARGET_HOST_H
