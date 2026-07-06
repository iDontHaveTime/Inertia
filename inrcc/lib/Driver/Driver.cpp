// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/ADT/StrView.h>
#include <inr/Support/Compiler.h>
#include <inr/Support/Switch.h>
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
        case DriverMode::CXX:
            return false;
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
    if(args.has(Arg::Kind::Target)) {
        Arg* target = args.getLast(Arg::Kind::Target);

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

        DriverMode dmode = inr::StrSwitch<DriverMode>(modeStr)
                               .newCase("gcc", DriverMode::CC)
                               .newCase("g++", DriverMode::CXX)
                               .setDefault(DriverMode::Unknown);

        if(dmode == DriverMode::Unknown) {
            logerr("unknown value '", modeStr, "' in '", arg->getOriginal(),
                   '\'');
            return true;
        }
        mode_ = dmode;
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

// GCC defines suffixes as follows:
// .c - C source
// .i - C source no PP
// .ii - C++ source no PP
// .h - C header, shouldn't compile or link
// C++ headers: .hh, .H, .hp, .hxx, .hpp, .HPP, .h++, .tcc
// C++ sources:
// .cc, .cp, .cxx, .cpp, .CPP, .c++, .C
// Assembly sources:
// .s
// .S, .sx - must be preprocessed
// Others should be passed to the linker
#define INRCC_NEW_FILETYPE(str, type) \
    CexprStringKey<str, FileType, FileType::type> {}
constexpr CexprStringMap<
    FileType, INRCC_NEW_FILETYPE(".c", C), INRCC_NEW_FILETYPE(".i", CNOPP),
    INRCC_NEW_FILETYPE(".h", CHEADER), INRCC_NEW_FILETYPE(".cc", CXX),
    INRCC_NEW_FILETYPE(".cp", CXX), INRCC_NEW_FILETYPE(".cxx", CXX),
    INRCC_NEW_FILETYPE(".cpp", CXX), INRCC_NEW_FILETYPE(".CPP", CXX),
    INRCC_NEW_FILETYPE(".c++", CXX), INRCC_NEW_FILETYPE(".hh", CXXHEADER),
    INRCC_NEW_FILETYPE(".H", CXXHEADER), INRCC_NEW_FILETYPE(".hp", CXXHEADER),
    INRCC_NEW_FILETYPE(".hxx", CXXHEADER),
    INRCC_NEW_FILETYPE(".hpp", CXXHEADER),
    INRCC_NEW_FILETYPE(".HPP", CXXHEADER),
    INRCC_NEW_FILETYPE(".h++", CXXHEADER),
    INRCC_NEW_FILETYPE(".tcc", CXXHEADER), INRCC_NEW_FILETYPE(".ii", CXXNOPP),
    INRCC_NEW_FILETYPE(".C", CXX), INRCC_NEW_FILETYPE(".S", PassToAssembler),
    INRCC_NEW_FILETYPE(".sx", PassToAssembler),
    INRCC_NEW_FILETYPE(".s", PassToAssemblerNoPP)>
    fileTypeMap;
#undef INRCC_NEW_FILETYPE

FileType Driver::getFileType(Arg* arg) {
    if(ftoverride_ != FileType::Unknown) return ftoverride_;
    inr::sview ext = arg->getOriginal();

    auto pos = ext.findLast('.');
    if(pos == inr::sview::npos) {
        return FileType::PassToLinker;
    }
    else {
        ext = ext.substr(pos, inr::sview::npos);
    }

    const FileType* ft = fileTypeMap.find(ext);

    if(ft) return *ft;
    return FileType::PassToLinker;
}

static inline bool isSourceFileType(FileType ft) {
    switch(ft) {
        case FileType::C:
        case FileType::CXX:
        case FileType::CNOPP:
        case FileType::CXXNOPP:
            return true;
        default:
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

    Sema sema;
    Parser parser(lex, arena, sema, diagnostics);
    auto decls = parser.parseAll();

    diagnostics.printall(inr::logs());

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
            else if(opt == "c-header") {
                ftoverride_ = FileType::CHEADER;
            }
            else if(opt == "cpp-output") {
                ftoverride_ = FileType::CNOPP;
            }
            else if(opt == "c++-cpp-output") {
                ftoverride_ = FileType::CXXNOPP;
            }
            else if(opt == "assembler") {
                ftoverride_ = FileType::PassToAssemblerNoPP;
            }
            else if(opt == "assembler-with-cpp") {
                ftoverride_ = FileType::PassToAssembler;
            }
            else {
                logwarn("unknown language '", opt, "' defaulting to 'none'");
            }
            continue;
        }

        Arg* arg = &argRef;
        FileType fileType = getFileType(arg);

        if(fileType == FileType::Unknown) {
            return 1;
        }
        if(!isSourceFileType(fileType)) {
            continue;
        }

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