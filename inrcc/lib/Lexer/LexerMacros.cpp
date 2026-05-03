// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/Target/Triple.h>
#include <inrcc/Lexer/Lexer.h>
#include <inrcc/Options/Data.h>

#include <inrcc/Lexer/Macros.inc>

namespace inrcc {

MACRO_WITH_INTEGER_VALUE(MacroN1, 1);
MACRO_WITH_INTEGER_VALUE(C99N, 199901L);
MACRO_WITH_INTEGER_VALUE(C11N, 201112L);
MACRO_WITH_INTEGER_VALUE(C17N, 201710L);
MACRO_WITH_INTEGER_VALUE(C23N, 202311L);

MACRO_WITH_INTEGER_VALUE_NON_BUILTIN(MacroN1NB, 1);

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
#define STR_STR_LEN(str) str, (sizeof(str) - 1)
    auto entry = infoTable_.find(STR_STR_LEN("__has_include"));
    if(entry) {
        MacroInfo* has_inc = arena_.alloc<MacroInfo>();
        has_inc->getReplacements().emplace_back(*entry);
        macros_.insert(STR_STR_LEN("__has_include"), has_inc);
    }
    entry = infoTable_.find(STR_STR_LEN("__has_include_next"));
    if(entry) {
        MacroInfo* has_inc = arena_.alloc<MacroInfo>();
        has_inc->getReplacements().emplace_back(*entry);
        macros_.insert(STR_STR_LEN("__has_include_next"), has_inc);
    }
#undef STR_STR_LEN
}

struct IdentMapTypes {
    using IdentPtr = IdentInfo*;
    IdentPtr chart, shortt, intt, longt, signedt, unsignedt;

    IdentMapTypes(IdentMap& map) {
        auto c = map.find("char", 4);
        auto s = map.find("short", 5);
        auto i = map.find("int", 3);
        auto l = map.find("long", 4);
        auto ss = map.find("signed", 6);
        auto us = map.find("unsigned", 8);

        if(c) chart = *c;
        if(s) shortt = *s;
        if(i) intt = *i;
        if(l) longt = *l;
        if(ss) signedt = *ss;
        if(us) unsignedt = *us;
    }
};

static inline void setBasedOnClosestType(const IdentMapTypes& types,
                                         MacroInfo::MacroReplacements& to,
                                         const CData& data, unsigned request,
                                         int sign) {
    // sign is 0 none, -1 signed, 1 unsigned
    if(request == data.getCharWidth()) {
        if(sign == 0) {
            to.emplace_back(types.chart);
        }
        else if(sign < 0) {
            to.emplace_back(types.signedt);
            to.emplace_back(types.chart);
        }
        else if(sign > 0) {
            to.emplace_back(types.unsignedt);
            to.emplace_back(types.chart);
        }
    }
    else if(request == data.getShortWidth()) {
        if(sign == 0) {
            to.emplace_back(types.shortt);
        }
        else if(sign < 0) {
            to.emplace_back(types.signedt);
            to.emplace_back(types.shortt);
        }
        else if(sign > 0) {
            to.emplace_back(types.unsignedt);
            to.emplace_back(types.shortt);
        }
    }
    else if(request == data.getIntWidth()) {
        if(sign == 0) {
            to.emplace_back(types.intt);
        }
        else if(sign < 0) {
            to.emplace_back(types.signedt);
            to.emplace_back(types.intt);
        }
        else if(sign > 0) {
            to.emplace_back(types.unsignedt);
            to.emplace_back(types.intt);
        }
    }
    else if(request == data.getLongWidth()) {
        if(sign == 0) {
            to.emplace_back(types.longt);
            to.emplace_back(types.intt);
        }
        else if(sign < 0) {
            to.emplace_back(types.signedt);
            to.emplace_back(types.longt);
            to.emplace_back(types.intt);
        }
        else if(sign > 0) {
            to.emplace_back(types.unsignedt);
            to.emplace_back(types.longt);
            to.emplace_back(types.intt);
        }
    }
    else if(request == data.getLongLongWidth()) {
        if(sign == 0) {
            to.emplace_back(types.longt);
            to.emplace_back(types.longt);
            to.emplace_back(types.intt);
        }
        else if(sign < 0) {
            to.emplace_back(types.signedt);
            to.emplace_back(types.longt);
            to.emplace_back(types.longt);
            to.emplace_back(types.intt);
        }
        else if(sign > 0) {
            to.emplace_back(types.unsignedt);
            to.emplace_back(types.longt);
            to.emplace_back(types.longt);
            to.emplace_back(types.intt);
        }
    }
}

void Lexer::setTypeMacros(const CData& data) {
    if(data.getIntWidth() == 32 && data.getPtrWidth() == 64 &&
       data.getLongLongWidth() == 64 && data.getLongWidth() == 64) {
        INSERT_MACRO("_LP64", MacroN1);
        INSERT_MACRO("__LP64__", MacroN1);
    }

    IdentMapTypes types(infoTable_);

#define SET_TYPE_MACRO_NEW(ID, STR, COND, SIGN) \
    MacroInfo* ID = arena_.alloc<MacroInfo>();  \
    ID->enableBuiltin();                        \
    macros_.insert(STR, sizeof(STR) - 1, ID);   \
    setBasedOnClosestType(types, ID->getReplacements(), data, COND, SIGN)

#define ALIAS_TYPE_MACRO(ID, STR) macros_.insert(STR, sizeof(STR) - 1, ID);

    SET_TYPE_MACRO_NEW(sizetype, "__SIZE_TYPE__", data.getSizeWidth(), 1);

    // wchar, wint, intmax, uintmax, sig_atomic, skip for now.
    SET_TYPE_MACRO_NEW(int8, "__INT8_TYPE__", 8, -1);
    SET_TYPE_MACRO_NEW(int16, "__INT16_TYPE__", 16, -1);
    SET_TYPE_MACRO_NEW(int32, "__INT32_TYPE__", 32, -1);
    SET_TYPE_MACRO_NEW(int64, "__INT64_TYPE__", 64, -1);

    SET_TYPE_MACRO_NEW(uint8, "__UINT8_TYPE__", 8, 1);
    SET_TYPE_MACRO_NEW(uint16, "__UINT16_TYPE__", 16, 1);
    SET_TYPE_MACRO_NEW(uint32, "__UINT32_TYPE__", 32, 1);
    SET_TYPE_MACRO_NEW(uint64, "__UINT64_TYPE__", 64, 1);

    // Alias both least and fast for now.
    ALIAS_TYPE_MACRO(int8, "__INT_LEAST8_TYPE__");
    ALIAS_TYPE_MACRO(int16, "__INT_LEAST16_TYPE__");
    ALIAS_TYPE_MACRO(int32, "__INT_LEAST32_TYPE__");
    ALIAS_TYPE_MACRO(int64, "__INT_LEAST64_TYPE__");

    ALIAS_TYPE_MACRO(uint8, "__UINT_LEAST8_TYPE__");
    ALIAS_TYPE_MACRO(uint16, "__UINT_LEAST16_TYPE__");
    ALIAS_TYPE_MACRO(uint32, "__UINT_LEAST32_TYPE__");
    ALIAS_TYPE_MACRO(uint64, "__UINT_LEAST64_TYPE__");

    ALIAS_TYPE_MACRO(int8, "__INT_FAST8_TYPE__");
    ALIAS_TYPE_MACRO(int16, "__INT_FAST16_TYPE__");
    ALIAS_TYPE_MACRO(int32, "__INT_FAST32_TYPE__");
    ALIAS_TYPE_MACRO(int64, "__INT_FAST64_TYPE__");

    ALIAS_TYPE_MACRO(uint8, "__UINT_FAST8_TYPE__");
    ALIAS_TYPE_MACRO(uint16, "__UINT_FAST16_TYPE__");
    ALIAS_TYPE_MACRO(uint32, "__UINT_FAST32_TYPE__");
    ALIAS_TYPE_MACRO(uint64, "__UINT_FAST64_TYPE__");

    SET_TYPE_MACRO_NEW(intptr, "__INTPTR_TYPE__", data.getPtrWidth(), 0);
    SET_TYPE_MACRO_NEW(uintptr, "__UINTPTR_TYPE__", data.getPtrWidth(), 1);

    // use pointer size at least for now
    ALIAS_TYPE_MACRO(intptr, "__PTRDIFF_TYPE__");

    char* buffer = nullptr;
    std::to_chars_result cres;

#define NUMERICAL_MACRO_NEW(ID, STR, VAL)                          \
    buffer = arena_.allocN<char>(16);                              \
    MacroInfo* ID = arena_.alloc<MacroInfo>();                     \
    ID->enableBuiltin();                                           \
    macros_.insert(STR, sizeof(STR) - 1, ID);                      \
    cres = std::to_chars(buffer, buffer + 16, VAL);                \
    if(cres.ec == std::errc())                                     \
    ID->getReplacements().emplace_back(TokenKind::LITERAL_INTEGER, \
                                       cres.ptr - buffer, buffer, nullptr)

    NUMERICAL_MACRO_NEW(shortw, "__SHRT_WIDTH__", data.getShortWidth());
    NUMERICAL_MACRO_NEW(intw, "__INT_WIDTH__", data.getIntWidth());
    NUMERICAL_MACRO_NEW(lw, "__LONG_WIDTH__", data.getLongWidth());
    NUMERICAL_MACRO_NEW(llw, "__LONG_LONG_WIDTH__", data.getLongLongWidth());

    NUMERICAL_MACRO_NEW(sizew, "__SIZE_WIDTH__", data.getSizeWidth());

    NUMERICAL_MACRO_NEW(intlw8, "__INT_LEAST8_WIDTH__", 8);
    NUMERICAL_MACRO_NEW(intlw16, "__INT_LEAST16_WIDTH__", 16);
    NUMERICAL_MACRO_NEW(intlw32, "__INT_LEAST32_WIDTH__", 32);
    NUMERICAL_MACRO_NEW(intlw64, "__INT_LEAST64_WIDTH__", 64);

    ALIAS_TYPE_MACRO(intlw8, "__INT_FAST8_WIDTH__");
    ALIAS_TYPE_MACRO(intlw16, "__INT_FAST16_WIDTH__");
    ALIAS_TYPE_MACRO(intlw32, "__INT_FAST32_WIDTH__");
    ALIAS_TYPE_MACRO(intlw64, "__INT_FAST64_WIDTH__");

    NUMERICAL_MACRO_NEW(intptrw, "__INTPTR_WIDTH__", data.getPtrWidth());

    // pointer size currently
    ALIAS_TYPE_MACRO(intptrw, "__PTRDIFF_WIDTH__");

    NUMERICAL_MACRO_NEW(shortsz, "__SIZEOF_SHORT__", data.getShortSize());
    NUMERICAL_MACRO_NEW(intsz, "__SIZEOF_INT__", data.getIntSize());
    NUMERICAL_MACRO_NEW(longsz, "__SIZEOF_LONG__", data.getLongSize());
    NUMERICAL_MACRO_NEW(llsz, "__SIZEOF_LONG_LONG__", data.getLongLongSize());
    NUMERICAL_MACRO_NEW(ptrsz, "__SIZEOF_POINTER__", data.getPtrSize());
    NUMERICAL_MACRO_NEW(sizesz, "__SIZEOF_SIZE_T__", data.getSizeSize());

    // same scenario here
    ALIAS_TYPE_MACRO(ptrsz, "__SIZEOF_PTRDIFF_T__");
}

void Lexer::setMacrosBasedOnTriple(inr::Triple target) {
    inr::Triple::FileType format = target.getFileType();

    STARTIF(ARCH(x86_64))
    INSERT_MACRO("__x86_64__", MacroN1);
    INSERT_MACRO("__x86_64", MacroN1);
    INSERT_MACRO("__amd64__", MacroN1);
    INSERT_MACRO("__amd64", MacroN1);

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

// Used for -D
void Lexer::addMacroWithPredefOne(inr::sview view) {
    macros_.insert(view.data(), view.size(), &MacroN1NB);
}

} // namespace inrcc