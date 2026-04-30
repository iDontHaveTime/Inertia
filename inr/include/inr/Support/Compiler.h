// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_COMPILER_H
#define INERTIA_SUPPORT_COMPILER_H

/// @file Support/Compiler.h
/// @brief Provides miscellaneous macros.

#ifndef __has_feature
#define __has_feature(x) 0
#endif

#ifndef __has_extension
#define __has_extension(x) 0
#endif

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#ifndef __has_warning
#define __has_warning(x) 0
#endif

#ifdef __GNUC__
#define inr_useattr(x) [[gnu::x]]
#else
#define inr_useattr(x)
#endif

#ifndef NDEBUG
#include <cassert>

#define inr_assert(expr, str) assert((expr) && (str))
#define inr_notpossible(str) inr_assert(false, str)

#else

#define inr_assert(expr, str) ((void)0)

#if __has_builtin(__builtin_unreachable)
#define inr_notpossible(str) __builtin_unreachable()
#else
#define inr_notpossible(str) ((void)0)
#endif

#endif

#endif // INERTIA_SUPPORT_COMPILER_H
