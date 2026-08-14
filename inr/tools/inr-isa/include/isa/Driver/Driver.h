// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_DRIVER_DRIVER
#define INERTIA_DRIVER_DRIVER

/// @file Driver/Driver.h
/// @brief Provides the `inr-isa` driver.

#include <inr/Support/Stream.h>

namespace isa {

class Driver {
    template<typename... Args>
    static void printError(Args&&... args) {
        ((inr::err() << "inr-isa: ").changeColor(inr::col::RED, true)
         << "error: ")
            .resetColor();
        ((inr::err() << args), ...);
        inr::err() << '\n';
    }

    static void printHelp();
    static void printVersion();

public:
    int isaMain(int, char**);
};

} // namespace isa

#endif // INERTIA_DRIVER_DRIVER
