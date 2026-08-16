// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_UNREACHABLE_H
#define INERTIA_SUPPORT_UNREACHABLE_H

/// @file Support/Unreachable.h
/// @brief Provides the unreachable macro.

#ifndef NDEBUG

namespace inr::internal {

[[noreturn]]
void inr_unreachable_impl(const char* file, long line, const char* func,
                          const char* str);

}

#if defined(__GNUC__) || defined(__clang__)
#define INR_UNREACHABLE_FUNC                                              \
    __PRETTY_FUNCTION__ /* Should assert and unreachable share this under \
                           Support/Shared/Macros.h */
#else
#define INR_UNREACHABLE_FUNC __func__
#endif

#define inr_unreachable(str)                                \
    inr::internal::inr_unreachable_impl(__FILE__, __LINE__, \
                                        INR_UNREACHABLE_FUNC, str)

#else

#if defined(_MSC_VER) && !defined(__clang__)
#define inr_unreachable(str) __assume(false)
#else
#define inr_unreachable(str) __builtin_unreachable()
#endif

#endif

#endif // INERTIA_SUPPORT_UNREACHABLE_H
