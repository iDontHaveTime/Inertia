// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Support/StrStream.h>
#include <inr/Support/Stream.h>

int main() {
    inr::sstream str;
    str << "inr::sstream";

    if(str.access() != "inr::sstream") return 1;

    return 0;
}
