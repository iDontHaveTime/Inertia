// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_PRINTER
#define INERTIA_TIR_PRINTER

/// @file TIR/Printer.h
/// @brief Prints out readable TIR.

#include <inr/Support/Stream.h>
#include <inr/TIR/TModule.h>
#include <inr/Target/TargetDesc.h>

namespace inr {

class TIRPrinter {
    const TModule& mod_;
    const TargetInfo* tinfo_;

public:
    TIRPrinter(const TModule& mod, const TargetInfo* tinfo) :
        mod_(mod), tinfo_(tinfo) {}

    void print(stream&) const;
    static void printType(stream& os, TIRT t);
};

} // namespace inr

#endif // INERTIA_TIR_PRINTER
