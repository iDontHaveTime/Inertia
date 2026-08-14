// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_X86_X86DEFS
#define INERTIA_X86_X86DEFS

/// @file x86/x86Defs.h
/// @brief Provides the x86 definitions to use in the implementation files.

#include <inr/TIR/CCLowering.h>
#include <inr/Target/Register.h>
#include <inr/Target/TargetDesc.h>

#include "x86Features.inc"
#include "x86Registers.inc"

namespace inr::x86 {

extern TargetRegisters x86_registers;

const TIRCCLowering* getAMD64SysVCC();

} // namespace inr::x86

#endif // INERTIA_X86_X86DEFS
