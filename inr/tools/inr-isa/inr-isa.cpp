// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <isa/Driver/Driver.h>

int main(int argc, char** argv) {
    isa::Driver driver;
    return driver.isaMain(argc, argv);
}
