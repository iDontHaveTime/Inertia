// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Target/Host.h>
#include <inr/Target/Target.h>

namespace inr::host {

Arch getArch() {
#if defined(__x86_64__)
    return Arch::x86_64;
#else
    return Arch::Error;
#endif
}

OS getOS() {
#if defined(__linux__)
    return OS::Linux;
#else
    return OS::Unknown;
#endif
}

ABI getABI() {
#if defined(__gnu_linux__)
    return ABI::GNU;
#else
    return ABI::None;
#endif
}

Target getTarget() {
    return Target(getArch(), getOS(), getABI());
}

} // namespace inr::host
