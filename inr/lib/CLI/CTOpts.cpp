// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/ADT/ArrView.h>
#include <inr/CLI/CTOpts.h>

#include <string_view>

namespace inr {

void ctOptPrint(stream& os, const CTOptArg* args, std::size_t num) {
    constexpr static unsigned INDENTATION = 28;
    constexpr std::string_view JOINED_SUFFIX = "<value>";

    const CTOptArg* end = args + num;
    for(; args != end; args++) {
        std::size_t totalSize = 2 + args->signature.size();

        os.indent(2) << args->signature;
        if(args->opt.getOptKind() == CTOpt::Joined) {
            os << JOINED_SUFFIX;
            totalSize += JOINED_SUFFIX.size();
        }

        if(totalSize < INDENTATION) {
            os.indent(INDENTATION - totalSize);
        }
        else {
            (os << '\n').indent(INDENTATION);
        }

        os << args->helpText << '\n';
    }
}

std::optional<CTOptVal> ctOptMatch(std::string_view arg1, std::string_view arg2,
                                   const CTOptArg* args, std::size_t num) {
    inr::arrview<CTOptArg> opts{args, num};
    for(const CTOptArg& opt : opts) {
        if(arg1.starts_with(opt.signature)) {
            switch(opt.opt.getOptKind()) {
                case CTOpt::Flag:
                    if(arg1.size() == opt.signature.size())
                        return {
                            CTOptVal(opt.opt.getKind(), false, opt.signature)};
                    break;
                case CTOpt::Joined:
                    return {CTOptVal(opt.opt.getKind(), false,
                                     arg1.substr(opt.signature.size()))};
                case CTOpt::JoinedSeparate:
                    if(arg1.size() > opt.signature.size()) {
                        return {CTOptVal(opt.opt.getKind(), false,
                                         arg1.substr(opt.signature.size()))};
                    }
                    [[fallthrough]];
                case CTOpt::Separate:
                    if(arg1.size() == opt.signature.size())
                        return {CTOptVal(opt.opt.getKind(), true, arg2)};
                    break;
            }
        }
    }
    return std::nullopt;
}

void ctOptParse(const CTOptArg* args, std::size_t num, int argc, char** argv,
                char sharedPrefix, uint32_t inputOpt, uint32_t stdinOpt,
                uint32_t unknownOpt, std::vector<CTOptVal>& opts_,
                unsigned argPos_[], uint16_t hasArg_[]) {
    if(argc <= 1) return;
    opts_.reserve(argc - 1);
    for(int i = 1; i < argc; i++) {
        std::string_view arg(argv[i]);
        if(arg[0] == sharedPrefix) {
            if(arg.size() == 1) {
                argPos_[stdinOpt] = opts_.size();
                hasArg_[stdinOpt]++;
                opts_.emplace_back(stdinOpt, false, arg);
            }
            else {
                std::string_view arg2 =
                    ((i + 1) < argc) ? argv[i + 1] : std::string_view{};
                auto maybeOpt = ctOptMatch(arg, arg2, args, num);
                if(maybeOpt.has_value()) {
                    const CTOptVal& opt = maybeOpt.value();
                    argPos_[opt.optType] = opts_.size();
                    hasArg_[opt.optType]++;
                    opts_.emplace_back(opt);
                    if(opt.arg2used) i++;
                }
                else {
                    argPos_[unknownOpt] = opts_.size();
                    hasArg_[unknownOpt]++;
                    opts_.emplace_back(unknownOpt, false, arg);
                }
            }
        }
        else {
            argPos_[inputOpt] = opts_.size();
            hasArg_[inputOpt]++;
            opts_.emplace_back(inputOpt, false, arg);
        }
    }
}

} // namespace inr
