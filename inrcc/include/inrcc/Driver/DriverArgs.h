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
    inr::sview original_;
    std::string optional_; ///< Processed string, optional.

    Kind kind_;

public:
    Arg(inr::sview original, std::string optional, Kind kind) :
        original_(original), optional_(optional), kind_(kind) {}

    const std::string& getOptional() const noexcept {
        return optional_;
    }

    std::string moveOptional() noexcept {
        return std::move(optional_);
    }

    inr::sview getOriginal() const noexcept {
        return original_;
    }

    Kind getKind() const noexcept {
        return kind_;
    }
};

class ArgOpt {
public:
    enum class OptKind { Flag, Joined, Separate, JoinedOrSeparate };

    Arg::Kind kind_;
    OptKind optKind_;

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
