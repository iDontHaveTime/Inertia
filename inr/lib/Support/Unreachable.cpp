// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Support/Unreachable.h>

#include <cstdio>
#include <cstdlib>

// Same reason as Assert.cpp

namespace inr::internal {

[[noreturn]]
void inr_unreachable_impl(const char* file, long line, const char* func,
                          const char* str) {
    std::fprintf(stderr,
                 "Unreachable reached\n"
                 "location: %s:%ld\n"
                 "function: %s\n"
                 "message: %s\n",
                 file, line, func, str);
    std::abort();
}

} // namespace inr::internal
