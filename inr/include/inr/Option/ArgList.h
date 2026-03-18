// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_OPTION_ARGLIST_H
#define INERTIA_OPTION_ARGLIST_H

/// @file Option/ArgList.h
/// @brief Contains the arg list class.

#include <inr/Option/Arg.h>
#include <inr/Option/Option.h>

#include <string>
#include <vector>

namespace inr::opt {

class ArgList {
    std::vector<Arg> args_;

    ArgList() noexcept = default;

    void newArg(sview str, Option* opt, std::vector<const char*> values) {
        args_.push_back(Arg(str, opt, args_.size(), std::move(values)));
    }

public:
    ArgList(const ArgList&) = delete;
    ArgList& operator=(const ArgList&) = delete;

    ArgList(ArgList&&) noexcept = default;
    ArgList& operator=(ArgList&&) noexcept = default;

    ~ArgList() noexcept = default;

    const std::vector<Arg>& getArgs() const noexcept {
        return args_;
    }

    bool hasArg(Option::OptionID id) {
        return getArg(id) != nullptr;
    }

    Arg* getArg(Option::OptionID id) {
        Arg* a = getArgNoClaim(id);

        if(a) a->claim();

        return a;
    }

    Arg* getArgNoClaim(Option::OptionID id) {
        for(Arg& arg : args_) {
            if(arg.getID() == id) return &arg;
        }
        return nullptr;
    }

    std::vector<std::string> getAllArgsValues(Option::OptionID id) const {
        std::vector<std::string> vec;
        for(const Arg& arg : args_) {
            if(arg.getID() == id) {
                vec.push_back(arg.getArgStr().str());
            }
        }
        return vec;
    }

    void printHelp();

    friend class OptionTable;
};

} // namespace inr::opt

#endif // INERTIA_OPTION_ARGLIST_H
