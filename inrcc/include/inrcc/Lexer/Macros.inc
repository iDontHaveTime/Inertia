// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#define MACRO_WITH_INTEGER_VALUE(ID, N) \
static MacroInfo ID{{Token(TokenKind::LITERAL_INTEGER, (sizeof(#N)-1), #N, nullptr)}, true}

#define MACRO_WITH_INTEGER_VALUE_NON_BUILTIN(ID, N) \
static MacroInfo ID{{Token(TokenKind::LITERAL_INTEGER, (sizeof(#N)-1), #N, nullptr)}}

#define MACRO_WITH_STRING_VALUE(ID, STR) \
static MacroInfo ID{{Token(TokenKind::LITERAL_STRING, (sizeof(STR)-1), STR, nullptr)}, true}

#define INSERT_MACRO(STR, MACRO) \
macros_.insert(STR, sizeof(STR)-1, &MACRO)

#define LANG(COND) lang_.COND
#define STANDARD(STANDARD) LANG(getStandard() & Language::STANDARD)
#define TARGET(COND) target.COND
#define ARCH(COND) TARGET(getArch() == inr::Triple::Arch::COND)
#define OS(COND) TARGET(getOS() == inr::Triple::OS::COND)
#define ABI(COND) TARGET(getABI() == inr::Triple::ABI::COND)
#define FORMAT(COND) format == inr::Triple::FileType::COND
#define LOR(CONDA, CONDB) (CONDA) || (CONDB)
#define ENDIAN(COND) target.getEndian() == std::endian::COND

#define STARTIF(COND) if(COND) {

#define ELSEIF(COND) } else if(COND) {
    
#define ELSE() } else {
#define ENDIF() }