// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef NEW_ISA_OPT
#define NEW_ISA_OPT(...)
#endif

NEW_ISA_OPT(Help, "--help", Flag, "Print available options.")
NEW_ISA_OPT(Version, "--version", Flag, "Prints out the Inertia version.")
NEW_ISA_OPT(PrintParsed, "--print-expr", Flag,
            "Prints out the parsed expressions.")
NEW_ISA_OPT(PrintEmitters, "--print-emitters", Flag,
            "Prints out the available emitters.")
NEW_ISA_OPT(ChooseEmitter, "-emitter=", Joined, "Chooses the emitter to use.")
NEW_ISA_OPT(Output, "-o", JoinedSeparate, "Outputs C++ to the file provided.")
NEW_ISA_OPT(IncludeLicense, "--include-license", Flag,
            "Includes the Inertia license in the file.")
NEW_ISA_OPT(HelpEmitter, "-emitter-help=", Joined,
            "Prints out the usage of an emitter.")
