// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/Support/Switch.h>
#include <inr/Support/Version.h>
#include <inr/Target/Triple.h>
#include <inrcc/ADT/CexprStringMap.h>
#include <inrcc/Driver/Driver.h>
#include <inrcc/Driver/DriverArgs.h>

#include <filesystem>

namespace inrcc {

Language Driver::getDefaultLanguage() {
    switch(mode_) {
        case DriverMode::CC:
            return Language::c99; // Currently c99 is the default.
        case DriverMode::CXX:
        case DriverMode::Unknown:
            return Language::Unknown;
    }
}

bool Driver::verifyArgs(class ArgVec& args) {
    bool err = false;

#define INRCC_FLAG(IDENT, STR, HELP_TEXT, OPTKIND, ...)             \
    if constexpr(ArgOpt::OptKind::OPTKIND == ArgOpt::OptKind::Flag) \
        if(args.howMany(Arg::Kind::IDENT) > 1) {                    \
            logwarn("redundant '" STR "' flag");                    \
        }
#define INRCC_FLAG_ALIAS(...)
#include <inrcc/Driver/Flags.inc>
#undef INRCC_FLAG_ALIAS
#undef INRCC_FLAG

    if(args.howMany(Arg::Kind::Output) > 1) {
        logerr("more than one output specified");
        err = true;
    }

    if(args.howMany(Arg::Kind::Standard) > 1) {
        logerr("more than one standard specified");
        err = true;
    }

    if(args.howMany(Arg::Kind::ModeSet) > 1) {
        logerr("more than one mode specified");
        err = true;
    }

    if(args.howMany(Arg::Kind::Sysroot) > 1) {
        logerr("more than one sysroot specified");
        err = true;
    }

    if(args.howMany(Arg::Kind::Target) > 1) {
        logerr("more than one target triple specified");
        err = true;
    }

    if(args.has(Arg::Kind::Output) && args.has(Arg::Kind::NoLink) &&
       args.howMany(Arg::Kind::Input) > 1) {
        logerr("only one input file can be chosen when using '-c' and '-o'");
        err = true;
    }

    return err;
}

#define INRCC_FLAG(IDENT, STR, ...) \
    constexpr inr::sview INRCC_FLAG_##IDENT##_STR = STR;
#define INRCC_FLAG_ALIAS(...)
#include <inrcc/Driver/Flags.inc>
#undef INRCC_FLAG_ALIAS
#undef INRCC_FLAG

static inline void printHelpHelper(inr::sview flag, inr::sview help_text,
                                   ArgOpt::OptKind optkind, bool alias) {
    constexpr static unsigned INDENTATION = 28;
    constexpr inr::sview JOINED_SUFFIX = "<value>";
    size_t totalSize = 2 + flag.size();

    inr::outs().indent(2) << flag;
    if(optkind == ArgOpt::OptKind::Joined) {
        inr::outs() << JOINED_SUFFIX;
        totalSize += JOINED_SUFFIX.size();
    }

    if(totalSize < INDENTATION) {
        inr::outs().indent(INDENTATION - totalSize);
    }
    else {
        (inr::outs() << '\n').indent(INDENTATION);
    }

    if(!alias) {
        inr::outs() << help_text << '\n';
    }
    else {
        inr::outs() << "Alias for '" << help_text << "'.\n";
    }
}

void Driver::printHelp() {
    inr::outs() << "Usage: " << toolName_
                << " [options] <file...> -o <file>\nOptions:\n";
#define INRCC_FLAG(IDENT, STR, HELP_TEXT, OPTKIND, ...) \
    printHelpHelper(STR, HELP_TEXT, ArgOpt::OptKind::OPTKIND, false);
#define INRCC_FLAG_ALIAS(IDENT, STR, OPTKIND, ...)                           \
    printHelpHelper(STR, INRCC_FLAG_##IDENT##_STR, ArgOpt::OptKind::OPTKIND, \
                    true);
#include <inrcc/Driver/Flags.inc>
#undef INRCC_FLAG_ALIAS
#undef INRCC_FLAG
}

void Driver::printVersion() {
    inr::outs() << "inrcc version: " << inr::getInertiaVersion() << '\n'
                << "Target: " << target_ << '\n';
}

DriverMode getModeFromArgv0(const char* arg) {
    inr::sview argS(arg);
    if(argS.size() < 2) return DriverMode::Unknown;

    argS = argS.substr(argS.size() - 2, 2);

    DriverMode dmode = inr::StrSwitch<DriverMode>(argS)
                           .newCase("cc", DriverMode::CC)
                           .newCase("++", DriverMode::CXX)
                           .setDefault(DriverMode::Unknown);

    return dmode;
}

std::string getToolNameFromArgv0(const char* arg) {
    return std::filesystem::path(arg).filename().string();
}

constexpr CexprStringMap<ArgOpt
#define NEW_CEXPR_OPT(IDENT, STR, OPTKIND) \
    , CexprStringKey<STR, ArgOpt,          \
                     ArgOpt{Arg::Kind::IDENT, ArgOpt::OptKind::OPTKIND}> {}
#define INRCC_FLAG(IDENT, STR, HELP_TEXT, OPTKIND, ...) \
    NEW_CEXPR_OPT(IDENT, STR, OPTKIND)
#define INRCC_FLAG_ALIAS(IDENT, STR, OPTKIND, ...) \
    NEW_CEXPR_OPT(IDENT, STR, OPTKIND)
#include <inrcc/Driver/Flags.inc>
#undef INRCC_FLAG_ALIAS
#undef INRCC_FLAG
#undef NEW_CEXPR_OPT
                         >
    inrccOpts;

bool Driver::matchJoined(int i, std::vector<Arg>& args) {
    const ArgOpt* opt = nullptr;
    size_t offset = 0;
    inr::sview arg = argv_[i];
#define MATCH_JOINED(STR, OPTKIND)                                      \
    if constexpr(ArgOpt::OptKind::OPTKIND == ArgOpt::OptKind::Joined || \
                 ArgOpt::OptKind::OPTKIND ==                            \
                     ArgOpt::OptKind::JoinedOrSeparate) {               \
        if(arg.size() >= (sizeof(STR) - 1)) {                           \
            opt = inrccOpts.find(arg.substr(0, (sizeof(STR) - 1)));     \
            if(opt) {                                                   \
                offset = sizeof(STR) - 1;                               \
                goto matching_done;                                     \
            }                                                           \
        }                                                               \
    }
#define INRCC_FLAG(IDENT, STR, HELP_TEXT, OPTKIND, ...) \
    MATCH_JOINED(STR, OPTKIND)
#define INRCC_FLAG_ALIAS(IDENT, STR, OPTKIND) MATCH_JOINED(STR, OPTKIND)
#include <inrcc/Driver/Flags.inc>
#undef INRCC_FLAG_ALIAS
#undef INRCC_FLAG
#undef MATCH_JOINED
matching_done:
    if(opt) {
        switch(opt->getOptKind()) {
            case ArgOpt::OptKind::JoinedOrSeparate:
                [[fallthrough]];
            case ArgOpt::OptKind::Joined:
                args.emplace_back(arg, arg.slice(offset, arg.size()).str(),
                                  opt->getKind());
                break;
            case ArgOpt::OptKind::Separate:
            case ArgOpt::OptKind::Flag:
                inr_notpossible(
                    "Separate and flag are not checked thus are impossible.");
        }
    }

    return opt != nullptr;
}

std::vector<Arg> Driver::parseArgs(bool& err) {
    if(argc_ == 0) {
        inr::log::send(inr::errs(), inr::log::Level::ERROR, "inrcc",
                       "no argv[0] is present");
        err = true;
        return {};
    }

    err = false;

    std::vector<Arg> args;
    args.reserve(argc_);

    mode_ = getModeFromArgv0(argv_[0]);

    args.emplace_back(argv_[0], std::string(), Arg::Kind::Tool);
    toolName_ = getToolNameFromArgv0(argv_[0]);

    for(int i = 1; i < argc_; i++) {
        inr::sview arg(argv_[i]);

        if(arg[0] != '-') {
            args.emplace_back(arg, std::string(), Arg::Kind::Input);
            continue;
        }

        if(arg.size() > 1) {
            const ArgOpt* opt = inrccOpts.find(arg);
            if(opt) {
                switch(opt->getOptKind()) {
                    case ArgOpt::OptKind::Flag:
                        args.emplace_back(arg, std::string(), opt->getKind());
                        continue;
                    case ArgOpt::OptKind::Separate:
                        [[fallthrough]];
                    case ArgOpt::OptKind::JoinedOrSeparate:
                        if(i + 1 == argc_) {
                            logerr("flag '", arg,
                                   "' is missing a value after it");
                            err = true;
                            continue;
                        }
                        args.emplace_back(arg, argv_[++i], opt->getKind());
                        break;
                    case ArgOpt::OptKind::Joined:
                        // Lets say it detects '-std=' it means there is no
                        // value after it.
                        logerr("flag '", arg, "' is missing a value after it");
                        break;
                }
            }
            else {
                if(!matchJoined(i, args)) {
                    logerr("unknown flag'", arg, '\'');
                    err = true;
                }
            }
        }
        else {
            args.emplace_back(arg, std::string(), Arg::Kind::Input).setOpt(1);
        }
    }

    return args;
}

} // namespace inrcc