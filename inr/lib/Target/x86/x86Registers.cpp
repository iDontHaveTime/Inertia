// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#define TARGET_REGISTER_DEFINITIONS
#include "x86Defs.h"

namespace inr::x86 {

TargetRegisters x86_registers{target_registers, target_register_families,
                              target_register_classes};

}
