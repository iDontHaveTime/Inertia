// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifdef INRCC_FLAG

INRCC_FLAG(Help, "--help", "Prints out the available flags.", Flag)
INRCC_FLAG(Version, "--version", "Prints out the version of the compiler.", Flag)
INRCC_FLAG(Output, "-o", "Specifies the output file.", JoinedOrSeparate)
INRCC_FLAG(Include, "-I", "Adds an include directory.", JoinedOrSeparate)
INRCC_FLAG(Standard, "-std=", "Sets the standard of the current language.", Joined)
INRCC_FLAG(Nostdinc, "-nostdinc", "Doesn't include standard headers.", Flag)
INRCC_FLAG(ISystem, "-isystem", "Adds a system include directory.", JoinedOrSeparate)
INRCC_FLAG(ModeSet, "--driver-mode=", "Sets the driver mode of the compiler.", Joined)
INRCC_FLAG(Sysroot, "--sysroot=", "Sets the root to look for includes in.", Joined)
INRCC_FLAG(Define, "-D", "Defines a macro for the preprocessor.", JoinedOrSeparate)
INRCC_FLAG(Define_Alias, "--define-macro=", "An alias for '-D'.", Joined)
INRCC_FLAG(Target, "--target=", "Sets the compilation target.", Joined)
INRCC_FLAG(Target_Alias, "-target", "Alias for '--target='.", Separate)

#endif