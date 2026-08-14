// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Target/Host.h>
#include <inr/Target/Target.h>

namespace inr::host {

Target getTarget() {
    Arch arch = Arch::Error;
    OS os = OS::Unknown;
    ABI abi = ABI::None;

#if defined(__x86_64__)
    arch = Arch::x86_64;
#endif

#if defined(__linux__)
    os = OS::Linux;
#endif

#if defined(__gnu_linux__)
    abi = ABI::GNU;
#endif

    return Target(arch, os, abi);
}

} // namespace inr::host
