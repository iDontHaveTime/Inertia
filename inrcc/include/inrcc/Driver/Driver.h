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
#include <inrcc/Diagnostics/Diagnostics.h>
#include <inrcc/Options/LangOptions.h>

#include <vector>

namespace inrcc {

/// @brief Possible modes the driver can be in.
enum class DriverMode {
    Unknown, ///< Unknown mode.
    CC       ///< C compiler.
};

/// @brief Represents file types that the driver knows.
enum class FileType {
    Unknown,        ///< Unknown file type.
    C,              ///< C files: .c
    CXX,            ///< C++ files: .cpp, .cxx, .cc, .C
    PassToLinker,   ///< Binary files: .o, .a, .so
    PassToAssembler ///< Assembly files: .s, .S, .asm
};

/// @brief The main entry point handler for inrcc.
class Driver {
    int argc_;                ///< Original argc.
    DriverMode mode_;         ///< Current mode.
    char** argv_;             ///< Original argv.
    inr::Triple target_;      ///< Compilation target.
    Diagnostics diagnostics_; ///< Diagnostics.

    std::string toolName_; ///< Name of the tool, typically inrcc.

    /// @brief Gets the default language for current mode.
    Language getDefaultLanguage();
    /// @brief An internal function that parses the args from argc and argv.
    std::vector<class Arg> parseArgs(bool& err);
    /// @brief An internal function for parseArgs().
    bool matchJoined(int i, std::vector<Arg>& args);
    /// @brief An internal function that verifies args correctness.
    bool verifyArgs(class ArgVec&);

    /// @brief Prints out help, for --help.
    void printHelp();
    /// @brief Prints out the version, for --version.
    void printVersion();

    /// @brief Resolves the mode based on the args.
    bool resolveMode(ArgVec& args);
    /// @brief Resolves the language based on the args.
    bool resolveLanguage(ArgVec& args, Language& lang);
    /// @brief Resolves the target based on the args.
    bool resolveTarget(ArgVec& args);

    /// @brief Gets the file type based on the arg's suffix.
    FileType getFileType(Arg* arg);

    /// @brief Internal function for the compilation step.
    int sourceFileCompilation(ArgVec& args, Language lang);

public:
    Driver(int argc, char** argv) noexcept :
        argc_(argc),
        mode_(DriverMode::Unknown),
        argv_(argv),
        target_(inr::Triple::getDefaultTriple()) {}

    /// @brief Should be called in main().
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
