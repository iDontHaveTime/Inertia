// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

/// @file tools/inr-gen.cpp
/// @brief The inertia gen frontend tool.
///
/// This tool takes `.ing` (Inertia gen) files and generates C++ files to
/// include. Its mainly used for description of targets, such as their
/// instructions, endian, pointer size, etc..

#include <inr/Gen/Driver.h>
#include <inr/Gen/Record.h>

int main(int argc, char** argv) {
    /// Should be replaced with inr-gen generated opts.
    /// inr-gen should also be able to generate options and be used with the
    /// inr/Option/* headers.

    inr::gen::GenDriver driver(argc, argv);

    if(driver.parseArgs()) {
        driver.error("failed to parse args");
        return 1;
    }
    inr::gen::RecordStorage result;
    if(driver.driveFile(result)) {
        return 1;
    }

    if(driver.emit(result)) {
        driver.error("failed to emit via backend");
        return 1;
    }

    return 0;
}