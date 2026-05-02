// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inrcc/Lexer/Lexer.h>

namespace inrcc {

#define INRCC_KEYWORD(ID, ...)                                        \
    static IdentInfo inrcc_keyword_##ID(TokenKind::KEYWORD_##ID, #ID, \
                                        (sizeof(#ID) - 1));
#include <inrcc/Lexer/TokenKind.inc>
#undef INRCC_KEYWORD

static IdentInfo inrcc_keyword___VA_ARGS__(TokenKind::KEYWORD___VA_ARGS__,
                                           "__VA_ARGS__",
                                           (sizeof("__VA_ARGS__") - 1));

void Lexer::setKeywords() {
#define INRCC_SHARED_KEYWORD(ID, ...)                \
    infoTable_.insert(inrcc_keyword_##ID.getStart(), \
                      inrcc_keyword_##ID.getLength(), &inrcc_keyword_##ID);
#include <inrcc/Lexer/TokenKind.inc>
#undef INRCC_SHARED_KEYWORD
    infoTable_.insert(inrcc_keyword___VA_ARGS__.getStart(),
                      inrcc_keyword___VA_ARGS__.getLength(),
                      &inrcc_keyword___VA_ARGS__);
    switch(lang_.getStandard()) {
        case Language::gnu89:
        case Language::c89:
#define INRCC_KEYWORD_C89(ID, ...)                   \
    infoTable_.insert(inrcc_keyword_##ID.getStart(), \
                      inrcc_keyword_##ID.getLength(), &inrcc_keyword_##ID);
#include <inrcc/Lexer/TokenKind.inc>
#undef INRCC_KEYWORD_C89
            break;
        case Language::gnu99:
            [[fallthrough]];
        case Language::c99:
#define INRCC_KEYWORD_C99(ID, ...)                   \
    infoTable_.insert(inrcc_keyword_##ID.getStart(), \
                      inrcc_keyword_##ID.getLength(), &inrcc_keyword_##ID);
#include <inrcc/Lexer/TokenKind.inc>
#undef INRCC_KEYWORD_C99
            break;
        case Language::c11:
        case Language::c17:
        case Language::c23:
        case Language::Unknown:
            break;
            break;
    }
}

IdentInfo* MacroInfo::getDefaultMacroIdentVararg() {
    return &inrcc_keyword___VA_ARGS__;
}

} // namespace inrcc