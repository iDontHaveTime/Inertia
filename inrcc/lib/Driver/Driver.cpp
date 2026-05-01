// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/ADT/StrView.h>
#include <inr/Support/Compiler.h>
#include <inr/Target/Triple.h>
#include <inrcc/ADT/CexprStringMap.h>
#include <inrcc/ADT/StringMap.h>
#include <inrcc/Driver/Driver.h>
#include <inrcc/Driver/DriverArgs.h>
#include <inrcc/Driver/DriverFileManager.h>
#include <inrcc/Lexer/Lexer.h>
#include <inrcc/Options/Data.h>
#include <inrcc/Options/LangOptions.h>
#include <inrcc/Support/Arena.h>

namespace inrcc {

#define langs(s) Language::Standard::s
constexpr static uint32_t CC_DM_STANDARDS =
    langs(c89) | langs(c99) | langs(c11) | langs(c17) | langs(c23);
#undef langs

bool languageSupportedByDriverMode(DriverMode dm, Language lang) {
    switch(dm) {
        case DriverMode::Unknown:
            return false;
        case DriverMode::CC:
            return CC_DM_STANDARDS & (uint32_t)lang.getStandard();
    }
}

int Driver::driverMain() {
    bool parseArgsErr;

    std::vector<Arg> argsVec = parseArgs(parseArgsErr);
    if(parseArgsErr) {
        logerr("failed to parse arguments");
        return 1;
    }

    ArgVec args(argsVec);

    if(args.has(Arg::Kind::Help)) {
        printHelp();
        return 0;
    }

    if(args.has(Arg::Kind::Version)) {
        printVersion();
        return 0;
    }

    if(verifyArgs(args)) {
        return 1;
    }

    if(resolveMode(args)) return 1;

    Language lang = getDefaultLanguage();
    if(resolveLanguage(args, lang)) return 1;

    if(resolveTarget(args)) return 1;

    if(!args.has(Arg::Kind::Input)) {
        logerr("no input files");
        return 1;
    }

    if(!args.has(Arg::Kind::Output)) {
        logerr("no output specified");
        return 1;
    }

    return sourceFileCompilation(args, lang);
}

bool Driver::resolveTarget(ArgVec& args) {
    if(args.has(Arg::Kind::Target) || args.has(Arg::Kind::Target_Alias)) {
        Arg* target = args.getLast(Arg::Kind::Target);
        if(!target) target = args.getLast(Arg::Kind::Target_Alias);

        inr::Triple gotTriple = inr::Triple::fromString(target->getOptional());
        if(!gotTriple.validTriple()) return true;

        target_ = gotTriple;
    }

    return false;
}

bool Driver::resolveMode(ArgVec& args) {
    if(args.has(Arg::Kind::ModeSet)) {
        Arg* arg = args.getLast(Arg::Kind::ModeSet);
        inr::sview modeStr = arg->getOptional();

        if(modeStr == "gcc") {
            mode_ = DriverMode::CC;
        }
        else {
            logerr("unknown value '", modeStr, "' in '", arg->getOriginal(),
                   '\'');
            return true;
        }
    }

    if(mode_ == DriverMode::Unknown) {
        logerr("unknown driver mode set");
        return true;
    }

    return false;
}

bool Driver::resolveLanguage(ArgVec& args, Language& lang) {
    if(args.has(Arg::Kind::Standard)) {
        Arg* arg = args.getLast(Arg::Kind::Standard);
        inr::sview standStr = arg->getOptional();

        lang = Language::getFromString(standStr);
        if(lang.getStandard() == Language::Unknown) {
            logerr("unknown value '", standStr, "' in '", arg->getOriginal(),
                   '\'');
            return true;
        }
    }

    if(!languageSupportedByDriverMode(mode_, lang)) {
        logerr("unknown standard '", lang.getAsString(), "' chosen");
        return true;
    }

    if(!lang.isSupported()) {
        logerr("the '", lang.getAsString(), "' standard is not yet supported");
        return true;
    }

    return false;
}

#define INRCC_NEW_FILETYPE(str, type) \
    CexprStringKey<str, FileType, FileType::type> {}
constexpr CexprStringMap<
    FileType, INRCC_NEW_FILETYPE(".c", C), INRCC_NEW_FILETYPE(".cc", CXX),
    INRCC_NEW_FILETYPE(".cpp", CXX), INRCC_NEW_FILETYPE(".cxx", CXX),
    INRCC_NEW_FILETYPE(".C", CXX), INRCC_NEW_FILETYPE(".s", PassToAssembler),
    INRCC_NEW_FILETYPE(".S", PassToAssembler),
    INRCC_NEW_FILETYPE(".asm", PassToAssembler),
    INRCC_NEW_FILETYPE(".o", PassToLinker),
    INRCC_NEW_FILETYPE(".a", PassToLinker),
    INRCC_NEW_FILETYPE(".so", PassToLinker)>
    fileTypeMap;
#undef INRCC_NEW_FILETYPE

FileType Driver::getFileType(Arg* arg) {
    inr::sview ext = arg->getOriginal();
    ext = ext.slice(ext.findLast('.'), ext.size());
    if(ext[0] != '.') {
        logerr("file '", ext, "' has no file extension");
        return FileType::Unknown;
    }

    const FileType* ft = fileTypeMap.find(ext);

    if(ft) return *ft;

    logerr("unknown file extension '", ext, "' in file '", arg->getOriginal(),
           '\'');
    return FileType::Unknown;
}

static inline bool isSourceFileType(FileType ft) {
    switch(ft) {
        case FileType::C:
        case FileType::CXX:
            return true;
        case FileType::Unknown:
        case FileType::PassToLinker:
        case FileType::PassToAssembler:
            return false;
    }
}

bool compileSourceFile(ArgVec& args, DriverFMan::File file, Language lang,
                       DriverFMan& fman, inr::Triple target) {
    Arena arena;
    IdentMap infoTable;
    MacroInfo baseFile({{TokenKind::LITERAL_STRING, 0, nullptr, nullptr}});
    CData data(target);

    Lexer lex(lang, file, fman, arena, infoTable, &baseFile);
    lex.setMacrosBasedOnTriple(target);
    lex.setTypeMacros(data);

    auto predefines = args.get(Arg::Kind::Define);

    for(Arg* arg : predefines) {
        lex.addMacroWithPredefOne(arg->getOptional());
    }

    predefines = args.get(Arg::Kind::Define_Alias);

    for(Arg* arg : predefines) {
        lex.addMacroWithPredefOne(arg->getOptional());
    }

    Token tok;
    do {
        tok = lex.next();
        inr::outs() << tok << ' ';
    } while(tok.getKind() != TokenKind::TOKEN_END);
    inr::outs() << '\n';

    return false;
}

int Driver::sourceFileCompilation(ArgVec& args, Language lang) {
    DriverFMan fman;

    auto includePaths = args.get(Arg::Kind::Include);

    for(Arg* arg : includePaths) {
        fman.addIncludePath(arg->moveOptional());
    }

    auto isystemPaths = args.get(Arg::Kind::ISystem);

    for(Arg* arg : isystemPaths) {
        fman.addIncludePath(arg->moveOptional());
    }

    if(Arg* arg = args.getLast(Arg::Kind::Sysroot)) {
        fman.setSysroot(arg->moveOptional());
    }

    if(!args.getLast(Arg::Kind::Nostdinc)) {
        fman.addLinuxLikeIncludePaths();
    }

    auto sources = args.get(Arg::Kind::Input);

    for(Arg* arg : sources) {
        FileType fileType = getFileType(arg);
        if(fileType == FileType::Unknown) {
            return 1;
        }
        if(!isSourceFileType(fileType)) continue;

        DriverFMan::File file =
            fman.openFileBufferNoInclude(arg->getOriginal());
        if(!file.file) {
            logerr("file '", arg->getOriginal(),
                   "' doesn't exist or is a directory");

            return 1;
        }

        if(compileSourceFile(args, file, lang, fman, target_)) {
            return 1;
        }
    }

    return 0;
}

} // namespace inrcc