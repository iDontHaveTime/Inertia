// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_DRIVER_DRIVER_H
#define INRCC_DRIVER_DRIVER_H

/// @file Driver/Driver.h
/// @brief Contains the main driver class for inrcc.

#include <inr/ADT/StrView.h>
#include <inr/Support/Logger.h>
#include <inr/Target/Triple.h>
#include <inrcc/Options/LangOptions.h>

#include <vector>

namespace inrcc {

enum class DriverMode {
    Unknown, ///< Unknown mode.
    CC       ///< C compiler.
};

enum class FileType {
    Unknown,        ///< Unknown file type.
    C,              ///< C files: .c
    CXX,            ///< C++ files: .cpp, .cxx, .cc, .C
    PassToLinker,   ///< Binary files: .o, .a, .so
    PassToAssembler ///< Assembly files: .s, .S, .asm
};

class Driver {
    int argc_;
    DriverMode mode_;
    char** argv_;
    inr::Triple target_;

    std::string toolName_; ///< Name of the tool, typically inrcc.

    Language getDefaultLanguage();
    std::vector<class Arg> parseArgs(bool& err);
    bool matchJoined(int i, std::vector<Arg>& args);
    bool verifyArgs(class ArgVec&);

    void printHelp();
    void printVersion();

    bool resolveMode(ArgVec& args);
    bool resolveLanguage(ArgVec& args, Language& lang);
    bool resolveTarget(ArgVec& args);

    FileType getFileType(Arg* arg);

    int sourceFileCompilation(ArgVec& args, Language lang);

public:
    Driver(int argc, char** argv) noexcept :
        argc_(argc),
        mode_(DriverMode::Unknown),
        argv_(argv),
        target_(inr::Triple::getDefaultTriple()) {}

    int driverMain();

    template<typename... Args>
    void logerr(Args&&... args) const {
        inr::log::sendargs(inr::outs(), inr::log::Level::ERROR, toolName_,
                           std::forward<Args>(args)...);
    }

    template<typename... Args>
    void logwarn(Args&&... args) const {
        inr::log::sendargs(inr::outs(), inr::log::Level::WARN, toolName_,
                           std::forward<Args>(args)...);
    }
};

} // namespace inrcc

#endif // INRCC_DRIVER_DRIVER_H
