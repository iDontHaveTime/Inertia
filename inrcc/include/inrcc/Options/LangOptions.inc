// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

// Inrcc's languages
#ifdef INRCC_LANG

#define INRCC_EXT 0x8000

// C89
INRCC_LANG(c89, false, "c89", 0x1)
// C89 w Extensions
INRCC_LANG(gnu89, false, "gnu89", 0x1 | INRCC_EXT)

// C99
INRCC_LANG(c99, true, "c99", 0x2)
// C99 w Extensions
INRCC_LANG(gnu99, true, "gnu99", 0x2 | INRCC_EXT)

// C11
INRCC_LANG(c11, false, "c11", 0x4)

// C17
INRCC_LANG(c17, false, "c17", 0x8)

// C23
INRCC_LANG(c23, false, "c23", 0x10)

INRCC_LANG(Unknown, false, "unknown", 0)

#undef INRCC_EXT

#endif