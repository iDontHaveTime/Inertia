// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inrcc/Options/LangOptions.h>

namespace inrcc {

bool Language::isLanguageSupported(Standard lang) noexcept {
    switch(lang) {
#define INRCC_LANG(IDENT, SUPPORTED, ...) \
    case IDENT:                           \
        return SUPPORTED;
#include <inrcc/Options/LangOptions.inc>
#undef INRCC_LANG
    }
}

const char* Language::getAsString(Standard lang) noexcept {
    switch(lang) {
#define INRCC_LANG(IDENT, SUPPORTED, STR, ...) \
    case IDENT:                                \
        return STR;
#include <inrcc/Options/LangOptions.inc>
#undef INRCC_LANG
    }
}

Language Language::getFromString(inr::sview str) noexcept {
#define INRCC_LANG(IDENT, SUPPORTED, STR, ...) \
    if(str == STR) {                           \
        return {IDENT};                        \
    }                                          \
    else
#include <inrcc/Options/LangOptions.inc>
#undef INRCC_LANG
    return {};
}

} // namespace inrcc