// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inrcc/Driver/Driver.h>
#include <inrcc/Options/LangOptions.h>

int main(int argc, char** argv) {
    inrcc::Driver driver(argc, argv);

    return driver.driverMain();
}