// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_FLAG
#define INRCC_FLAG(...)
#define INRCC_FLAG_ALIAS(...)
#define INRCC_FLAG_UNDEF
#endif

INRCC_FLAG(Help, "--help", "Prints out the available flags.", Flag)

INRCC_FLAG(Version, "--version", "Prints out the version of the compiler.",
           Flag)

INRCC_FLAG(Output, "-o", "Specifies the output file.", JoinedOrSeparate)
INRCC_FLAG_ALIAS(Output, "--output=", Joined)
INRCC_FLAG_ALIAS(Output, "--output", Separate)

INRCC_FLAG(Include, "-I", "Adds an include directory.", JoinedOrSeparate)

INRCC_FLAG(NoLink, "-c", "Stops the compiler from linking.", Flag)
INRCC_FLAG_ALIAS(NoLink, "--compile", Flag)

INRCC_FLAG(NoAssemble, "-S", "Stops the compiler from assembling.", Flag)
INRCC_FLAG_ALIAS(NoAssemble, "--assemble", Flag)

INRCC_FLAG(OnlyPreprocess, "-E", "Stops after preprocessing.", Flag)
INRCC_FLAG_ALIAS(OnlyPreprocess, "--preprocess", Flag)

INRCC_FLAG(SetLanguage, "-x", "Overrides the language of the next input.",
           JoinedOrSeparate)
INRCC_FLAG_ALIAS(SetLanguage, "--language=", Joined)
INRCC_FLAG_ALIAS(SetLanguage, "--language", Separate)

INRCC_FLAG(Standard, "-std=", "Sets the standard of the current language.",
           Joined)

INRCC_FLAG(Nostdinc, "-nostdinc", "Doesn't include standard headers.", Flag)

INRCC_FLAG(ISystem, "-isystem", "Adds a system include directory.",
           JoinedOrSeparate)

INRCC_FLAG(ModeSet, "--driver-mode=", "Sets the driver mode of the compiler.",
           Joined)

INRCC_FLAG(Sysroot, "--sysroot=", "Sets the root to look for includes in.",
           Joined)

INRCC_FLAG(Define, "-D", "Defines a macro for the preprocessor.",
           JoinedOrSeparate)
INRCC_FLAG_ALIAS(Define, "--define-macro=", Joined)
INRCC_FLAG_ALIAS(Define, "--define-macro", Separate)

INRCC_FLAG(Target, "--target=", "Sets the compilation target.", Joined)
INRCC_FLAG_ALIAS(Target, "-target", Separate)

INRCC_FLAG(Freestanding, "-ffreestanding", "Assumes a not hosted environment.",
           Flag)
INRCC_FLAG_ALIAS(Freestanding, "-fno-hosted", Flag)

#ifdef INRCC_FLAG_UNDEF
#undef INRCC_FLAG_UNDEF
#undef INRCC_FLAG_ALIAS
#undef INRCC_FLAG
#endif