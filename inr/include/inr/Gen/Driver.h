// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_GEN_DRIVER_H
#define INERTIA_GEN_DRIVER_H

/// @file Gen/Driver.h
/// @brief The driver for the inr-gen tool.

#include <inr/ADT/StrView.h>
#include <inr/Gen/Lexer.h>
#include <inr/Gen/Parser.h>
#include <inr/Gen/Record.h>
#include <inr/Support/CFile.h>
#include <inr/Support/Logger.h>
#include <inr/Support/MemoryFile.h>

#include <filesystem>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace inr::gen {

class GenDriver {
    int argc_;
    char** argv_;

    std::vector<std::filesystem::path> includes_;
    /// @brief Holds a map based on open files
    ///
    /// CFile is the file descriptor opened.
    /// MemoryFile is the file mapped to memory.
    /// sview is the original file name.
    std::unordered_map<std::filesystem::path,
                       std::tuple<CFile, MemoryFile, sview>>
        open_;

    sview input_;
    sview output_;

public:
    GenDriver(int argc, char** argv) noexcept : argc_(argc), argv_(argv) {}

    int getArgc() const noexcept {
        return argc_;
    }

    char** getArgv() const noexcept {
        return argv_;
    }

    template<typename... Args>
    void error(Args&&... args) const {
        log::sendargs(errs(), log::Level::ERROR, "inr-gen",
                      std::forward<Args>(args)...);
    }

    enum class RequestErr {
        Success,     ///< No errors.
        AlreadyOpen, ///< File already open.
        Internal     ///< Internal error in one of the classes.
    };

    /// @brief Requests a file to be opened.
    /// @param fname Name of the file to open.
    /// @return A pair, first being the filed in memory, second the error code.
    /// @note Only one of that file can be opened.
    std::pair<MemoryFile*, RequestErr> requestFile(sview fname,
                                                   bool searchIncludes) {
        std::filesystem::path fpath;

        std::filesystem::path strAsPath =
            std::filesystem::path(fname.begin(), fname.end());
        if(!std::filesystem::exists(strAsPath)) {
            if(searchIncludes) {
                for(const std::filesystem::path& inc : includes_) {
                    std::filesystem::path f = inc / strAsPath;
                    if(std::filesystem::exists(f)) {
                        fpath = std::move(f);
                        break;
                    }
                }
            }

            if(fpath.empty()) return {nullptr, RequestErr::Internal};
        }
        else {
            fpath = std::move(strAsPath);
        }

        fpath = std::filesystem::absolute(fpath).lexically_normal();

        auto itf = open_.find(fpath);
        if(itf != open_.end()) {
            return {&std::get<1>(itf->second), RequestErr::AlreadyOpen};
        }

        CFile file(fpath.c_str(), "rb", true);
        if(!file) {
            return {nullptr, RequestErr::Internal};
        }

        fseek(file.getFile(), 0, SEEK_END);
        long inputFileSize = ftell(file.getFile());
        fseek(file.getFile(), 0, SEEK_SET);

        MemoryFile memf(file, inputFileSize, true);
        if(memf.empty()) {
            return {nullptr, RequestErr::Internal};
        }

        auto [it, inserted] = open_.emplace(
            fpath, std::make_tuple(std::move(file), std::move(memf), fname));

        if(!inserted) {
            return {&std::get<1>(it->second), RequestErr::AlreadyOpen};
        }

        return {&std::get<1>(it->second), RequestErr::Success};
    }

    RequestErr driveFileSpecific(sview name, gen::RecordStorage& result,
                                 bool searchIncludes) {
        auto p = requestFile(name, searchIncludes);
        if(p.second != RequestErr::Success) {
            return p.second;
        }

        gen::lexer lexr(name, p.first->begin(), p.first->end());

        std::vector<inr::gen::token> tokens = lexr.lex();

        if(gen::parser::parse(*this, tokens, result)) {
            error("parsing failed");
            return RequestErr::Internal;
        }

        return RequestErr::Success;
    }

    /// @brief Lexes and parses the input file.
    bool driveFile(gen::RecordStorage& result) {
        return driveFileSpecific(input_, result, false) != RequestErr::Success;
    }

private:
    sview joinedOrSeparateFlag(int& i) {
        sview arg = getArgv()[i];

        if(arg.size() > 2) {
            return arg.slice(2, arg.size());
        }

        if(i + 1 >= argc_) {
            error("expected path after '-", (getArgv()[i])[1], '\'');
            return {};
        }
        else {
            return getArgv()[++i];
        }
    }

    bool parseFlag(int& i) {
        sview arg = getArgv()[i];

        if(arg.size() <= 1) return true;

        char flagSep = arg[1];

        if(flagSep == 'o') {
            sview out = joinedOrSeparateFlag(i);
            if(out.empty()) return true;

            if(output_.empty()) {
                output_ = out;
            }
            else {
                error("more than one output file specified");
                return {};
            }
        }
        else if(flagSep == 'I') {
            sview out = joinedOrSeparateFlag(i);
            if(out.empty()) return true;
            std::filesystem::path Ipath(out.begin(), out.end());

            if(!std::filesystem::is_directory(Ipath)) {
                error("provided include path is not a directory '",
                      Ipath.native(), '\'');
                return true;
            }

            includes_.emplace_back(std::move(Ipath));
        }

        return false;
    }

public:
    bool parseArgs() {
        bool err = false;
        for(int i = 1; i < getArgc(); i++) {
            sview arg = getArgv()[i];
            if(arg.empty()) continue;

            if(arg[0] == '-') {
                if(parseFlag(i)) {
                    err = true;
                    break;
                }
            }
            else {
                if(input_.empty()) {
                    input_ = arg;
                }
                else {
                    error("more than one input file specified");
                    err = true;
                    break;
                }
            }
        }

        if(input_.empty()) {
            error("no input file");
            err = true;
        }

        if(output_.empty()) {
            error("no output file");
            err = true;
        }

        return err;
    }
};

} // namespace inr::gen

#endif // INERTIA_GEN_DRIVER_H
