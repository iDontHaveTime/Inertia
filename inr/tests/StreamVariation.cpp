// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Support/CStream.h>
#include <inr/Support/STLStream.h>
#include <inr/Support/StrStream.h>
#include <inr/Support/Stream.h>

#include <iostream>

int main() {
    inr::sstream str;
    str << "Hello, World!\n";

    inr::cstream cstdout(stdout, false);

    cstdout << str.access();
    cstdout.flush();

    inr::stlstream stlcout(std::cout);

    stlcout << str.access();
    stlcout.flush();

    return 0;
}
