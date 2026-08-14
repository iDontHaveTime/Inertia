// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Support/Assert.h>

#include <cstdio>
#include <cstdlib>

// This file should always define inr_assert_impl.
// The reason for it is that headers use inr_assert as well, thus this function
// can be called even if Inertia was compiled with NDEBUG.

namespace inr::internal {

[[noreturn]]
void inr_assert_impl(const char* file, long line, const char* func,
                     const char* expr, const char* msg) {
    std::fprintf(stderr,
                 "Assertion failed\n"
                 "location: %s:%ld\n"
                 "function: %s\n"
                 "expression: %s\n"
                 "message: %s\n",
                 file, line, func, expr, msg);
    std::abort();
}

} // namespace inr::internal
