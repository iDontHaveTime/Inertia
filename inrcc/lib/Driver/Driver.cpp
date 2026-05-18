// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/ADT/StrView.h>
#include <inr/Support/Compiler.h>
#include <inr/Target/Triple.h>
#include <inrcc/ADT/CexprStringMap.h>
#include <inrcc/ADT/StringMap.h>
#include <inrcc/Diagnostics/Diagnostics.h>
#include <inrcc/Driver/Driver.h>
#include <inrcc/Driver/DriverArgs.h>
#include <inrcc/Driver/DriverFileManager.h>
#include <inrcc/Lexer/Lexer.h>
#include <inrcc/Options/Data.h>
#include <inrcc/Options/LangOptions.h>
#include <inrcc/Parser/Parser.h>
#include <inrcc/Sema/Sema.h>
#include <inrcc/Support/Arena.h>

#include <cstdio>
#include <cstdlib>

namespace inrcc {

bool languageSupportedByDriverMode(DriverMode dm, Language lang) {
    switch(dm) {
        case DriverMode::Unknown:
            return false;
        case DriverMode::CC:
            return lang.getStandard() == Language::c99;
            // return lang.isC();
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

    if(args.has(Arg::Kind::Freestanding)) {
        lang.enableFreestanding();
    }

    return false;
}

#define INRCC_NEW_FILETYPE(str, type) \
    CexprStringKey<str, FileType, FileType::type> {}
constexpr CexprStringMap<
    FileType, INRCC_NEW_FILETYPE(".c", C), INRCC_NEW_FILETYPE(".cc", CXX),
    INRCC_NEW_FILETYPE(".cpp", CXX), INRCC_NEW_FILETYPE(".cxx", CXX),
    INRCC_NEW_FILETYPE(".C", CXX), INRCC_NEW_FILETYPE(".CC", CXX),
    INRCC_NEW_FILETYPE(".s", PassToAssembler), INRCC_NEW_FILETYPE(".CPP", CXX),
    INRCC_NEW_FILETYPE(".S", PassToAssembler),
    INRCC_NEW_FILETYPE(".asm", PassToAssembler),
    INRCC_NEW_FILETYPE(".o", PassToLinker),
    INRCC_NEW_FILETYPE(".a", PassToLinker),
    INRCC_NEW_FILETYPE(".so", PassToLinker)>
    fileTypeMap;
#undef INRCC_NEW_FILETYPE

FileType Driver::getFileType(Arg* arg) {
    if(ftoverride_ != FileType::Unknown) return ftoverride_;
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
                       DriverFMan& fman, Diagnostics& diagnostics,
                       inr::Triple target) {
    Arena arena;
    IdentMap infoTable;
    MacroInfo baseFile(
        {Token(TokenKind::LITERAL_STRING, file.originalName.size(),
               file.originalName.data(), nullptr)},
        true);
    CData data(target);

    Lexer lex(lang, file, fman, arena, infoTable, diagnostics, &baseFile, data);
    lex.setMacrosBasedOnTriple(target);

    auto predefines = args.get(Arg::Kind::Define);

    for(Arg* arg : predefines) {
        lex.addMacroWithPredefOne(arg->getOptional());
    }

    predefines = args.get(Arg::Kind::Define_Alias);

    for(Arg* arg : predefines) {
        lex.addMacroWithPredefOne(arg->getOptional());
    }

    Sema sema;
    Parser parser(lex, arena, sema, diagnostics);
    auto decls = parser.parseAll();

    diagnostics.printall(inr::outs());

    if(diagnostics.hadFatalErrors()) {
        return true;
    }

    return diagnostics.hadErrors();
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

    for(Arg& argRef : args.getAll()) {
        if(argRef.getKind() != Arg::Kind::Input &&
           argRef.getKind() != Arg::Kind::SetLanguage) {
            continue;
        }

        if(argRef.getKind() == Arg::Kind::SetLanguage) {
            const std::string& opt = argRef.getOptional();

            if(opt == "c") {
                ftoverride_ = FileType::C;
            }
            else if(opt == "none") {
                ftoverride_ = FileType::Unknown;
            }
            else if(opt == "c++") {
                ftoverride_ = FileType::CXX;
            }
            else {
                logwarn("unknown language '", opt, "' defaulting to none");
            }
            continue;
        }

        Arg* arg = &argRef;
        FileType fileType = getFileType(arg);
        if(fileType == FileType::Unknown) {
            return 1;
        }
        if(!isSourceFileType(fileType)) continue;

        DriverFMan::File file;

        if(!arg->getOpt()) {
            file = fman.openFileBufferNoInclude(arg->getOriginal());
            if(!file.file) {
                logerr("file '", arg->getOriginal(),
                       "' doesn't exist or is a directory");

                return 1;
            }
        }
        else {
            size_t size = 0x2000;
            size_t bytesRead = 0;
            char* buffer = (char*)std::malloc(size);

            while(true) {
                if(!buffer) {
                    logerr("bad alloc for when reading stdin");
                    return 1;
                }

                if(size_t read = std::fread(buffer + bytesRead, 1,
                                            size - bytesRead, stdin)) {
                    if((bytesRead += read) == size) {
                        buffer = (char*)std::realloc(buffer, size <<= 1);
                    }
                }
                else break;
            }

            buffer[bytesRead] = '\0';
            file = fman.newCustomFile(buffer, buffer + bytesRead, "<stdin>");
        }

        if(compileSourceFile(args, file, lang, fman, diagnostics_, target_)) {
            return 1;
        }
    }

    return 0;
}

} // namespace inrcc