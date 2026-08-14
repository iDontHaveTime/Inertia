// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_DRIVER_DRIVER
#define INERTIA_DRIVER_DRIVER

/// @file Driver/Driver.h
/// @brief Provides the `inr-randir` driver class.

#include <inr/Support/Stream.h>
#include <inr/Vfs/FStream.h>

#include <memory>

namespace randir {

class Driver {
    inr::stream* output_;
    std::unique_ptr<inr::vfsstream> maybeFile_;

    template<typename... Args>
    static void printError(Args&&... args) {
        ((inr::err() << "inr-randir: ").changeColor(inr::col::RED, true)
         << "error: ")
            .resetColor();
        ((inr::err() << args), ...);
        inr::err() << '\n';
    }

    static void printHelp();
    static void printVersion();

public:
    int randirMain(int argc, char** argv);
};

} // namespace randir

#endif // INERTIA_DRIVER_DRIVER
