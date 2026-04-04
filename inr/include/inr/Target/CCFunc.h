// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifdef INR_CCFUNC
#undef INR_CCFUNC
namespace inr {
using CCFunc = bool (*)(unsigned, const class Type*, class CCState&);
}
#endif