// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/Support/Stream.h>
#include <inr/Support/Version.h>

namespace inr {

raw_stream& operator<<(raw_stream& os, Version v) {
    return os << v.getMajor() << '.' << v.getMinor() << '.' << v.getPatch();
}

Version getInertiaVersion() noexcept {
    return Version(INERTIA_VERSION_MAJOR, INERTIA_VERSION_MINOR,
                   INERTIA_VERSION_PATCH);
}

raw_stream& reportInertiaVersion(raw_stream& os){
    return os << "Inertia version " << getInertiaVersion();
}

} // namespace inr
