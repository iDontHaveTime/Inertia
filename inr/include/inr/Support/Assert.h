// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_ASSERT_H
#define INERTIA_SUPPORT_ASSERT_H

/// @file Support/Assert.h
/// @brief Provides an assert function.

#ifndef NDEBUG
namespace inr::internal {

[[noreturn]]
void inr_assert_impl(const char* file, long line, const char* func,
                     const char* expr, const char* msg);

}

#if defined(__GNUC__) || defined(__clang__)
#define INR_ASSERT_FUNC __PRETTY_FUNCTION__
#else
#define INR_ASSERT_FUNC __func__
#endif

#define inr_assert(expr, msg)                                            \
    do {                                                                 \
        if(!(expr)) [[unlikely]] {                                       \
            inr::internal::inr_assert_impl(__FILE__, __LINE__,           \
                                           INR_ASSERT_FUNC, #expr, msg); \
        }                                                                \
    } while(0)

#else
#define inr_assert(expr, msg) (void)0

#endif

#endif // INERTIA_SUPPORT_ASSERT_H
