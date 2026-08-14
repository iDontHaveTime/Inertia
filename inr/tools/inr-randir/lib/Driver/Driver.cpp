// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/CLI/CTOpts.h>
#include <inr/IR/Printer.h>
#include <inr/IR/TUnit.h>
#include <inr/IR/Type.h>
#include <inr/IR/TypeMap.h>
#include <inr/Support/Stream.h>
#include <inr/Support/Version.h>
#include <inr/Vfs/Vfs.h>
#include <randir/Driver/Driver.h>
#include <randir/IR/IRGen.h>

#include <charconv>
#include <deque>
#include <string>
#include <vector>

namespace randir {

enum class RandIROptKind {
    Input,
    StdinInput,
    Unknown,
#define NEW_RANDIR_OPT(IDENT, ...) IDENT,
#include <randir/Opts/RandIROpts.inc>
#undef NEW_RANDIR_OPT
    RANDIR_LAST_OPT,
};

constexpr inr::CTOptsMap randIROpts = std::array{
#define NEW_RANDIR_OPT(IDENT, FLAGSTR, OPTKIND, HELPSTR)                 \
    inr::CTOptArg{                                                       \
        FLAGSTR,                                                         \
        inr::CTOpt{inr::CTOpt::OPTKIND, uint32_t(RandIROptKind::IDENT)}, \
        HELPSTR},
#include <randir/Opts/RandIROpts.inc>
#undef NEW_RANDIR_OPT
};

void Driver::printHelp() {
    inr::out() << "Usage: inr-randir [options...]\nOptions:\n";
    randIROpts.printHelp(inr::out());
}

void Driver::printVersion() {
    inr::out() << "Inertia inr-randir version " << inr::getInertiaVersion()
               << '\n';
}

static inline std::string& generateName(std::deque<std::string>& names,
                                        std::string_view prefix) {
    unsigned c = names.size();
    std::string& str = names.emplace_back(prefix);
    str += std::to_string(c);
    return str;
}

int Driver::randirMain(int argc, char** argv) {
    output_ = &inr::out();

    inr::CTOptParser<std::size_t(RandIROptKind::RANDIR_LAST_OPT)> optsParser(
        randIROpts, argc, argv, '-', uint32_t(RandIROptKind::Input),
        uint32_t(RandIROptKind::StdinInput), uint32_t(RandIROptKind::Unknown));

    if(optsParser.has(uint32_t(RandIROptKind::Help))) {
        printHelp();
        return 0;
    }

    if(optsParser.has(uint32_t(RandIROptKind::Version))) {
        printVersion();
        return 0;
    }

    bool err = false;

    if(optsParser.has(uint32_t(RandIROptKind::Input))) {
        err = true;
        printError("input files are not supported");
    }

    if(optsParser.has(uint32_t(RandIROptKind::StdinInput))) {
        err = true;
        printError("stdin input is not supported");
    }

    if(optsParser.has(uint32_t(RandIROptKind::Unknown))) {
        err = true;
        auto vec = optsParser.get(uint32_t(RandIROptKind::Unknown));
        for(auto unknownOpt : vec) {
            printError("unknown opt '", unknownOpt->value, '\'');
        }
    }

    if(optsParser.howMany(uint32_t(RandIROptKind::Output)) > 1) {
        err = true;
        printError("more than one output specified");
    }

    if(err) return 1;

    if(optsParser.has(uint32_t(RandIROptKind::Output))) {
        const inr::CTOptVal& out =
            optsParser.getLast(uint32_t(RandIROptKind::Output));

        auto& vfs = inr::vfs::getNativeFs();

        std::error_code ec;

        auto f = vfs.open(
            out.value, inr::vfs::OpenMode(inr::vfs::OWRITE | inr::vfs::OTRUNC),
            ec);
        if(!f) {
            printError("error opening '", out.value,
                       "' with message: ", ec.message());
            return 1;
        }

        maybeFile_ = std::make_unique<inr::vfsstream>(std::move(f));
    }

    if(maybeFile_) {
        output_ = maybeFile_.get();
    }

    unsigned functionCount = 1;

    if(optsParser.has(uint32_t(RandIROptKind::FunctionCount))) {
        const auto& funcC =
            optsParser.getLast(uint32_t(RandIROptKind::FunctionCount));
        auto res = std::from_chars(funcC.value.data(),
                                   funcC.value.data() + funcC.value.size(),
                                   functionCount);
        if(res.ec != std::errc()) {
            printError("flag '-function-count' ",
                       std::make_error_code(res.ec).message());
            return 1;
        }
    }

    unsigned argCount = 12;
    if(optsParser.has(uint32_t(RandIROptKind::ArgCount))) {
        const auto& argC =
            optsParser.getLast(uint32_t(RandIROptKind::ArgCount));
        auto res = std::from_chars(
            argC.value.data(), argC.value.data() + argC.value.size(), argCount);
        if(res.ec != std::errc()) {
            printError("flag '-arg-count' ",
                       std::make_error_code(res.ec).message());
            return 1;
        }
    }

    unsigned bitmax = 1024;
    if(optsParser.has(uint32_t(RandIROptKind::IntMax))) {
        const auto& bmax = optsParser.getLast(uint32_t(RandIROptKind::IntMax));
        auto res = std::from_chars(
            bmax.value.data(), bmax.value.data() + bmax.value.size(), bitmax);
        if(res.ec != std::errc()) {
            printError("flag '-arg-count' ",
                       std::make_error_code(res.ec).message());
            return 1;
        }
    }

    std::string_view unitName = "inr-randir-unit";

    if(optsParser.has(uint32_t(RandIROptKind::UnitName))) {
        const auto& unitN =
            optsParser.getLast(uint32_t(RandIROptKind::UnitName));
        unitName = unitN.value;
    }

    inr::TUnit unit(unitName);
    inr::TypeMap tm;
    IRGen gen(tm);

    std::deque<std::string> functionNames;

    for(unsigned i = 0; i < functionCount; i++) {
        unsigned funcArgCount = gen.getRand32(0, argCount);
        std::vector<const inr::Type*> argTypes;
        for(unsigned j = 0; j < funcArgCount; j++) {
            argTypes.emplace_back(gen.randomBasicType(bitmax));
        }
        const inr::FuncType* ft = tm.getFunc(gen.randomReturnType(bitmax),
                                             argTypes, gen.getRandBool());
        auto func =
            unit.createFunction(ft, generateName(functionNames, "function"),
                                gen.getRandLinkage(), gen.getRandExt());
        for(unsigned j = 0; j < funcArgCount; j++) {
            func->getArg(j)->setExt(gen.getRandExt());
        }
    }

    inr::IRPrinter printer(unit);
    printer.print(*output_);

    return 0;
}

} // namespace randir
