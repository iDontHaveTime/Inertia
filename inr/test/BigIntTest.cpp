// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/ADT/BigInt.h>
#include <inr/Support/Stream.h>

int main() {
    inr::bigint zeroint;

    inr::outs() << zeroint << '\n' << inr::bigint(128, 64) + inr::bigint(128, 6) << '\n';

    return 0;
}