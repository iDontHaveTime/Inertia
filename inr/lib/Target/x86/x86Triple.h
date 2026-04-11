// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
//
// THIS FILE SHOULD NOT BE IN `include` AND SHOULD ONLY BE USED IN Triple.cpp
// (and x86Triple.cpp).

#ifndef INERTIA_X86_X86TRIPLE_H
#define INERTIA_X86_X86TRIPLE_H

#include <inr/TIR/TIRLowering.h>

namespace inr::x86 {

const char* getTIRAsmStr();
const TIRTargetDesc* getTIRTargetDesc();
Triple::TIRMatcherFunc getTIRMatchingFunc();
const RegisterInfo* getRegisterInfo();
CCFunc getCCFunc(CallingConv cc, bool ret);
arrview<Register> getCalleeSaved(CallingConv cc);
arrview<Register> getCallerSaved(CallingConv cc);

} // namespace inr::x86

#endif // INERTIA_X86_X86TRIPLE_H
