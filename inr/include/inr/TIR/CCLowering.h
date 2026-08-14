// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_CCLOWERING
#define INERTIA_TIR_CCLOWERING

/// @file TIR/CCLowering.h
/// @brief Provides calling convention lowering for TIR.

#include <inr/IR/FuncDef.h>
#include <inr/IR/InstDef.h>
#include <inr/TIR/TBlock.h>
#include <inr/TIR/TSymbol.h>
#include <inr/TIR/Translator.h>

namespace inr {

class TIRCCLowering {
public:
    virtual ~TIRCCLowering() = default;

    virtual bool lowerReturn(TBlock* blk, const RetInst& ret, const FuncDef& fd,
                             const Translator& om) const = 0;

    virtual bool lowerArgs(TBlock* entry, const TSymbol* sym,
                           Translator&) const = 0;
};

} // namespace inr

#endif // INERTIA_TIR_CCLOWERING
