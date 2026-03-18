// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_OPTION_OPTIONTABLE_H
#define INERTIA_OPTION_OPTIONTABLE_H

/// @file Option/OptionTable.h
/// @brief Contains the option table class.

#include <inr/ADT/ArrView.h>
#include <inr/Option/Option.h>

namespace inr::opt {

/// @brief Options table for parsing args.
///
/// The table should ideally be Inertia gen generated, it shouldn't be manually
/// made. This is because it can be optimized much better since there is control
/// over how it outputs strings and whatnot.
class OptionTable {
    arrview<Option> options_;

public:
    OptionTable(arrview<Option> options) noexcept : options_(options) {}

    class ArgList parseArgs(int argc, char* const* argv,
                            unsigned missingArgsCount);
};

} // namespace inr::opt

#endif // INERTIA_OPTION_OPTIONTABLE_H
