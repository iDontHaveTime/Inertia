// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inrcc/Lexer/Lexer.h>

#include "inr/Target/Triple.h"

namespace inrcc {

#include <inrcc/Lexer/Macros.inc>

MACRO_WITH_INTEGER_VALUE(MacroN1, 1);
MACRO_WITH_INTEGER_VALUE(C99N, 199901L);
MACRO_WITH_INTEGER_VALUE(C11N, 201112L);
MACRO_WITH_INTEGER_VALUE(C17N, 201710L);
MACRO_WITH_INTEGER_VALUE(C23N, 202311L);

MACRO_WITH_INTEGER_VALUE(INRCC_BIG_ENDIAN, 4321);
MACRO_WITH_INTEGER_VALUE(INRCC_LITTLE_ENDIAN, 1234);

#define MACRO_WITH_INTEGER_MACRO(ID, MACRO) MACRO_WITH_INTEGER_VALUE(ID, MACRO)
MACRO_WITH_INTEGER_MACRO(INRCC_MAJ, INRCC_VERSION_MAJOR);
MACRO_WITH_INTEGER_MACRO(INRCC_MIN, INRCC_VERSION_MINOR);
MACRO_WITH_INTEGER_MACRO(INRCC_PAT, INRCC_VERSION_PATCH);

#define MACRO_VERSION \
    INRCC_VERSION_MAJOR.INRCC_VERSION_MINOR.INRCC_VERSION_PATCH
#define STRINGIFY_VERSION(x) #x
#define STRING_VERSION(V) STRINGIFY_VERSION(V)

#define INRCC__VERSION__MACRO "inrcc " STRING_VERSION(MACRO_VERSION)

MACRO_WITH_STRING_VALUE(INRCC_VERSION, STRING_VERSION(MACRO_VERSION));
MACRO_WITH_STRING_VALUE(INRCC__VERSION__, INRCC__VERSION__MACRO);

#define NEW_STDC_VERSION(MACRO) INSERT_MACRO("__STDC_VERSION__", MACRO)

void Lexer::setMacros() {
    // __FILE__, gets updated internally
    INSERT_MACRO("__FILE__", fileMacro_);

    // __BASE_FILE__ handled by the driver

    // __inrcc__
    INSERT_MACRO("__inrcc__", MacroN1);
    INSERT_MACRO("__inrcc_major__", INRCC_MAJ);
    INSERT_MACRO("__inrcc_minor__", INRCC_MIN);
    INSERT_MACRO("__inrcc_patch__", INRCC_PAT);
    INSERT_MACRO("__inrcc_patchlevel__", INRCC_PAT); // To match clang/gcc
    INSERT_MACRO("__inrcc_version__", INRCC_VERSION);

    // __VERSION__
    INSERT_MACRO("__VERSION__", INRCC__VERSION__);

    // __STDC__
    INSERT_MACRO("__STDC__", MacroN1);

    STARTIF(STANDARD(c99))
    NEW_STDC_VERSION(C99N);
    ELSEIF(STANDARD(c11))
    NEW_STDC_VERSION(C11N);
    ELSEIF(STANDARD(c17))
    NEW_STDC_VERSION(C17N);
    ELSEIF(STANDARD(c23))
    NEW_STDC_VERSION(C23N);
    ENDIF()
}

void Lexer::setMacrosBasedOnTriple(inr::Triple target) {
    inr::Triple::FileType format = target.getFileType();
    STARTIF(ARCH(x86_64))
    INSERT_MACRO("__x86_64__", MacroN1);
    INSERT_MACRO("__x86_64", MacroN1);
    INSERT_MACRO("__amd64__", MacroN1);
    INSERT_MACRO("__amd64", MacroN1);

    STARTIF(OS(Linux))
    INSERT_MACRO("__LP64__", MacroN1);
    INSERT_MACRO("_LP64", MacroN1);
    ENDIF()
    ENDIF()

    STARTIF(OS(Linux))
    INSERT_MACRO("__linux__", MacroN1);
    INSERT_MACRO("__linux", MacroN1);

    STARTIF(ABI(GNU))
    INSERT_MACRO("__gnu_linux__", MacroN1);
    ENDIF()
    ENDIF()

    STARTIF(FORMAT(ELF))
    INSERT_MACRO("__ELF__", MacroN1);
    ENDIF()

    STARTIF(ENDIAN(little))
    INSERT_MACRO("__BYTE_ORDER__", INRCC_LITTLE_ENDIAN);
    INSERT_MACRO("__LITTLE_ENDIAN__", MacroN1);
    ELSE()
    INSERT_MACRO("__BYTE_ORDER__", INRCC_BIG_ENDIAN);
    INSERT_MACRO("__BIG_ENDIAN__", MacroN1);
    ENDIF()

    INSERT_MACRO("__ORDER_LITTLE_ENDIAN__", INRCC_LITTLE_ENDIAN);
    INSERT_MACRO("__ORDER_BIG_ENDIAN__", INRCC_BIG_ENDIAN);
}

void Lexer::addMacroWithPredefOne(inr::sview view) {
    macros_.insert(view.data(), view.size(), &MacroN1);
}

} // namespace inrcc