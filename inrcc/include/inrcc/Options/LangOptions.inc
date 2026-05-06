// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

// Inrcc's languages
#ifndef INRCC_LANG
#define INRCC_LANG(...)
#define UNDEF_INRCC_LANG
#endif

// C89
INRCC_LANG(c89, false, "c89", "gnu89")

// C99
INRCC_LANG(c99, true, "c99", "gnu99")

// C11
INRCC_LANG(c11, false, "c11", "gnu11")

// C17
INRCC_LANG(c17, false, "c17", "gnu17")

// C23
INRCC_LANG(c23, false, "c23", "gnu23")


#ifdef UNDEF_INRCC_LANG
#undef UNDEF_INRCC_LANG
#undef INRCC_LANG
#endif