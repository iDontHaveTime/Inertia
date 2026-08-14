// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

/// This file contains the option macros for the `inr-randir` tool.

#ifndef NEW_RANDIR_OPT
#define NEW_RANDIR_OPT(...)
#endif

NEW_RANDIR_OPT(Help, "--help", Flag, "Prints out available opts.")
NEW_RANDIR_OPT(Version, "--version", Flag, "Prints out the version.")
NEW_RANDIR_OPT(Output, "-o", JoinedSeparate, "Redirects the output to a file.")
NEW_RANDIR_OPT(FunctionCount, "-function-count=", Joined,
               "Changes the amount of functions to generate.")
NEW_RANDIR_OPT(UnitName, "-unit-name=", Joined,
               "Changes the name of the TUnit created.")
NEW_RANDIR_OPT(ArgCount, "-arg-count=", Joined,
               "Specifies the maximum amount of function args.")
NEW_RANDIR_OPT(IntMax, "-integer-max=", Joined,
               "Maximum bitwidth of an integer.")
