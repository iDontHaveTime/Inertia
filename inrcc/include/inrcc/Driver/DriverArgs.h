// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_DRIVER_DRIVERARGS_H
#define INRCC_DRIVER_DRIVERARGS_H

/// @file Driver/DriverArgs.h
/// @brief This file contains arg-related classes.

#include <inr/ADT/IVector.h>
#include <inr/ADT/StrView.h>

#include <cstdint>
#include <vector>

namespace inrcc {

/// @brief Represents an arg for the driver.
class Arg {
public:
    enum class Kind : unsigned {
        Input = 0, ///< Input file.
        Tool,      ///< The name of this tool.
#define INRCC_FLAG(IDENT, ...) IDENT,
#include <inrcc/Driver/Flags.inc>
#undef INRCC_FLAG

        ArgsEnd
    };

private:
    inr::sview original_;  ///< Original string of the arg.
    std::string optional_; ///< Processed string, optional.

    Kind kind_; ///< Arg kind.

public:
    Arg(inr::sview original, std::string optional, Kind kind) :
        original_(original), optional_(optional), kind_(kind) {}

    /// @brief Returns the optional string, valid in some cases.
    const std::string& getOptional() const noexcept {
        return optional_;
    }

    /// @brief Moves the optional string, used for reducing copying.
    std::string moveOptional() noexcept {
        return std::move(optional_);
    }

    /// @brief Returns the original string.
    inr::sview getOriginal() const noexcept {
        return original_;
    }

    /// @brief Returns the arg's kind.
    Kind getKind() const noexcept {
        return kind_;
    }
};

/// @brief Represents an arg option.
///
/// Its mostly about telling how this arg should act, for example
/// `OptKind::Flag` would mean this arg is a flag, no value before or after.
class ArgOpt {
public:
    /// @brief Represents on how should this arg get parsed.
    enum class OptKind {
        Flag,            ///< Standalone arg, e.g. --help, --version.
        Joined,          ///< Joined arg, e.g. -std=, --target=.
        Separate,        ///< Separate arg, e.g. -target.
        JoinedOrSeparate ///< Joined or separate arg, e.g. -I, -o, -D.
    };

    Arg::Kind kind_;  ///< What kind of arg this option represents.
    OptKind optKind_; ///< What option kind is this.

    constexpr ArgOpt() noexcept = default;

    constexpr ArgOpt(Arg::Kind kind, OptKind optKind) noexcept :
        kind_(kind), optKind_(optKind) {}

    Arg::Kind getKind() const noexcept {
        return kind_;
    }

    OptKind getOptKind() const noexcept {
        return optKind_;
    }
};

/// @brief Wraps an std::vector of Arg into a more abstract interface.
class ArgVec {
    std::vector<Arg>& args_;
    unsigned argPos_[(unsigned)Arg::Kind::ArgsEnd]{};
    uint16_t hasArg_[(unsigned)Arg::Kind::ArgsEnd]{};

    void processArgs() noexcept {
        unsigned idx = 0;
        for(const Arg& arg : args_) {
            hasArg_[(unsigned)arg.getKind()]++;
            argPos_[(unsigned)arg.getKind()] = idx++;
        }
    }

public:
    ArgVec(std::vector<Arg>& args) noexcept : args_(args) {
        processArgs();
    }

    uint16_t howMany(Arg::Kind kind) const noexcept {
        return hasArg_[(unsigned)kind];
    }

    bool has(Arg::Kind kind) const noexcept {
        return howMany(kind);
    }

    Arg* getLast(Arg::Kind kind) noexcept {
        inr_assert(kind != Arg::Kind::Tool,
                   "You cannot get the tool kind through getLast().");
        return argPos_[(unsigned)kind] != 0 ? &args_[argPos_[(unsigned)kind]]
                                            : nullptr;
    }

    inr::ivec<Arg*, 4> get(Arg::Kind kind) {
        if(howMany(kind) <= 1) {
            Arg* arg = getLast(kind);
            return arg ? inr::ivec<Arg*, 4>({arg}) : inr::ivec<Arg*, 4>();
        }

        inr::ivec<Arg*, 4> argPtrs;

        for(Arg& arg : args_) {
            if(arg.getKind() == kind) argPtrs.emplace_back(&arg);
        }

        return argPtrs;
    }
};

} // namespace inrcc

#endif // INRCC_DRIVER_DRIVERARGS_H
