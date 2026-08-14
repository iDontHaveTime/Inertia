// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_CLI_CTOPTS_H
#define INERTIA_CLI_CTOPTS_H

/// @file Cli/CTOpts.h
/// @brief Provides a compile time map for cli opts.
///
/// This could also be a header-only library, maybe with CMake options.

#include <inr/Support/Assert.h>
#include <inr/Support/Stream.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

namespace inr {

/// @brief One opt for the compile time map.
///
/// Lets say you wanted to add these options:
/// - `-D` - for macros being joined.
/// - `-std=` - to set the standard.
/// - `--version` - to print out the version.
/// - `-x` - to set the language.
/// First of all create an enum, ideally with the underlying type of `uint32_t`.
/// So in this example it would be:
/// ```cpp
/// enum class ArgKind : uint32_t {
///     Macro, Standard, Version, Language
/// };
/// ```
/// So then the corresponding `CTOpt`s would be these:
/// ```cpp
/// constexpr CTOpt macroOpt(CTOpt::Joined, uint32_t(ArgKind::Macro));
/// constexpr CTOpt stdOpt(CTOpt::Joined, uint32_t(ArgKind::Standard));
/// constexpr CTOpt verOpt(CTOpt::Flag, uint32_t(ArgKind::Version));
/// constexpr CTOpt langOpt(CTOpt::JoinedSeparate, uint32_t(ArgKind::Language));
/// ```
class CTOpt {
public:
    enum OptKind : unsigned char {
        /// @brief A standalone flag.
        ///
        /// Flags such as `--help` or `--version` are considered standalone flag
        /// because they don't have a value after them.
        Flag,
        /// @brief Separate value opt.
        ///
        /// This means that this opt has a value after it, examples of it
        /// include stuff like `-x lang`.
        /// @note Compilers like clang make `-x lang` both joined and
        /// separate.
        Separate,
        /// @brief Joined value opt.
        ///
        /// This means that this opt has a value right after its name, like
        /// `-std=`.
        Joined,
        /// @brief Joined or separate value opt.
        ///
        /// This means that this opt can take a value right after it in the
        /// name, or the next arg after it.
        JoinedSeparate,
    };

private:
    /// @brief OptKind of this opt.
    OptKind optKind_;
    /// @brief What this opt represents.
    uint32_t kind_;

public:
    constexpr CTOpt(OptKind optKind, uint32_t kind) :
        optKind_(optKind), kind_(kind) {}

    constexpr OptKind getOptKind() const {
        return optKind_;
    }

    constexpr uint32_t getKind() const {
        return kind_;
    }
};

/// @brief Represents an cli opt.
struct CTOptArg {
    /// @brief The argument's signature, for example `--version`, `--help`,
    /// etc..
    std::string_view signature;
    /// @brief Opt type of this arg.
    CTOpt opt;
    /// @brief The text displayed when help is used.
    std::string_view helpText;
};

/// @brief This is returned when the option matched.
struct CTOptVal {
    /// @brief The option type that matched, not OptKind but rather the
    /// user-provided enum.
    uint32_t optType;
    /// @brief Was the second arg used when matched.
    /// @note This is set when the arg was matched and the opt was Separate
    /// (including JoinedSeparate).
    bool arg2used;
    /// @brief The value of this arg.
    ///
    /// This changes depends on the OptKind.
    /// If it was a flag the value is the arg itself.
    /// But for Separate, Joined, and JoinedSeparate the value is what goes
    /// after the arg.
    std::string_view value;
};

/// @brief Prints out help text for the args.
///
/// It does NOT print out `Usage: ...\n` or `Options:\n` it just prints out them
/// as a list.
/// This is made as a separate function so templates wont copy code that can be
/// reused across every opts map.
/// @note The last character it prints is a new line.
extern void ctOptPrint(stream& os, const CTOptArg* args, std::size_t num);

/// @brief Used internally by the opts map, do not use manually.
///
/// This is here so that the templates wont copy the same logic over and over.
extern std::optional<CTOptVal> ctOptMatch(std::string_view arg1,
                                          std::string_view arg2,
                                          const CTOptArg* args,
                                          std::size_t num);

/// @brief Provides a compile-time cli opts map.
///
/// Compile-time meaning that this is sorted at compile-time, not that it only
/// works at compile-time.
/// Example usage:
/// ```cpp
/// constexpr CTOptsMap cli_opts = std::array{CTOptArg{
///    "-x", inr::CTOpt{inr::CTOpt::JoinedSeparate,
///    uint32_t(ArgKind::Language)},
///    "Interprets the next files as another language"}
/// };
/// ```
/// It is also a good idea to consider x macros in this case.
template<std::size_t OptCount>
class CTOptsMap {
    std::array<CTOptArg, OptCount> opts_;

public:
    constexpr CTOptsMap(std::array<CTOptArg, OptCount> opts) : opts_(opts) {
        std::sort(opts_.begin(), opts_.end(), [](CTOptArg a, CTOptArg b) {
            if(a.signature.size() != b.signature.size())
                return a.signature.size() > b.signature.size();
            return a.signature < b.signature;
        });
    }

    constexpr const std::array<CTOptArg, OptCount>& getOpts() const {
        return opts_;
    }

    /// @brief Matches an arg with the provided 2 args.
    /// @param arg1 The main arg to match.
    /// @param arg2 Optional second arg.
    ///
    /// If you lets say have argc 5 and you are on argc 4 then arg2 can just be
    /// {}.
    std::optional<CTOptVal> match(std::string_view arg1,
                                  std::string_view arg2) const {
        return ctOptMatch(arg1, arg2, opts_.data(), opts_.size());
    }

    void printHelp(stream& os) const {
        ctOptPrint(os, opts_.data(), opts_.size());
    }
};

/// @brief Used internally by CTOptParser, do not use manually.
/// @note This is much more explained in CTOptParser.
extern void ctOptParse(const CTOptArg* args, std::size_t num, int argc,
                       char** argv, char sharedPrefix, uint32_t inputOpt,
                       uint32_t stdinOpt, uint32_t unknownOpt,
                       std::vector<CTOptVal>& opts_, unsigned argPos_[],
                       uint16_t hasArg_[]);

/// @brief A simple cli opts parser that uses the CTOptsMap.
template<std::size_t EnumArgCount>
class CTOptParser {
    std::vector<CTOptVal> opts_;
    unsigned argPos_[EnumArgCount]{};
    uint16_t hasArg_[EnumArgCount]{};

public:
    /// @brief Parses the args and creates a vector of them internally.
    /// @param map The map to match args.
    /// @param argc Argc from main, starts from 1.
    /// @param argv Argv from main.
    /// @param sharedPrefix Prefix that all opts have, for example '-'.
    /// @param inputOpt When an arg doesn't start with sharedPrefix this opt is
    /// used.
    /// @param stdinOpt When an arg is sized 1 and is the sharedPrefix this opt
    /// is used.
    /// @param unknownOpt This opt is used when no match was found.
    template<std::size_t OptCount>
    CTOptParser(const CTOptsMap<OptCount>& map, int argc, char** argv,
                char sharedPrefix, uint32_t inputOpt, uint32_t stdinOpt,
                uint32_t unknownOpt) {
        ctOptParse(map.getOpts().data(), map.getOpts().size(), argc, argv,
                   sharedPrefix, inputOpt, stdinOpt, unknownOpt, opts_, argPos_,
                   hasArg_);
    }

    uint16_t howMany(uint32_t kind) const {
        return hasArg_[kind];
    }

    bool has(uint32_t kind) const {
        return howMany(kind);
    }

    const CTOptVal& getLast(uint32_t kind) const {
        inr_assert(
            has(kind),
            "CTOptParser getLast(): called but the opt isn't in the vector");
        return opts_[argPos_[kind]];
    }

    std::vector<const CTOptVal*> get(uint32_t kind) const {
        inr_assert(has(kind),
                   "CTOptParser get(): called but the opt isn't in the vector");

        std::vector<const CTOptVal*> opts;
        for(const CTOptVal& opt : opts_) {
            if(opt.optType == kind) {
                opts.emplace_back(&opt);
            }
        }

        return opts;
    }

    const std::vector<CTOptVal>& getAll() const {
        return opts_;
    }
};

} // namespace inr

#endif // INERTIA_CLI_CTOPTS_H
