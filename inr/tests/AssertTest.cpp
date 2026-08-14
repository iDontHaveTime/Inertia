// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <inr/Support/Assert.h>

#include <csignal>
#include <cstdlib>

void sig_abrt(int signal) {
    std::exit(0);
}

int main() {
    std::signal(SIGABRT, sig_abrt);

    inr_assert(true, "This should not fail");
    inr_assert(false, "This should fail");

    return 1;
}
