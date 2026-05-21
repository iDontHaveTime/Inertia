// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

// Inrcc's languages
#ifndef INRCC_LANG
#define INRCC_LANG(...)
#define INRCC_LANG_ALIAS(...)
#define UNDEF_INRCC_LANG
#endif

// C89
INRCC_LANG(c89, false, "c89", "gnu89")
INRCC_LANG_ALIAS(c89, "c90", "gnu90")
INRCC_LANG_ALIAS(c89, "iso9899:1990", "")

// C99
INRCC_LANG(c99, true, "c99", "gnu99")
INRCC_LANG_ALIAS(c99, "c9x", "gnu9x")
INRCC_LANG_ALIAS(c99, "iso9899:1999", "")
INRCC_LANG_ALIAS(c99, "iso9899:199x", "")

// C11
INRCC_LANG(c11, false, "c11", "gnu11")
INRCC_LANG_ALIAS(c11, "c1x", "gnu1x")
INRCC_LANG_ALIAS(c11, "iso9899:2011", "")

// C17
INRCC_LANG(c17, false, "c17", "gnu17")
INRCC_LANG_ALIAS(c17, "c18", "gnu18")
INRCC_LANG_ALIAS(c17, "iso9899:2017", "")
INRCC_LANG_ALIAS(c17, "iso9899:2018", "")

// C23
INRCC_LANG(c23, false, "c23", "gnu23")
INRCC_LANG_ALIAS(c23, "c2x", "gnu2x")
INRCC_LANG_ALIAS(c23, "iso9899:2024", "")

#ifdef UNDEF_INRCC_LANG
#undef UNDEF_INRCC_LANG
#undef INRCC_LANG_ALIAS
#undef INRCC_LANG
#endif