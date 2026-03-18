// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_SUPPORT_COMPILER_H
#define INERTIA_SUPPORT_COMPILER_H

/// @file Support/Compiler.h
/// @brief Provides macros that don't depend on the compiler.

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

#endif // INERTIA_SUPPORT_COMPILER_H
