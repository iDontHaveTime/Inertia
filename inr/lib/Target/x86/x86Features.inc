// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/Target/TargetDesc.h>
namespace inr::x86 {
struct CustomTargetFeatures : public TargetFeatures {
    bool long_mode = false;
    bool x87_fpu = true;
    bool sse = false;
    bool sse2 = false;
    bool sse3 = false;
    bool ssse3 = false;
    bool sse4_1 = false;
    bool sse4_2 = false;
    bool avx = false;
    bool avx2 = false;
};
}
