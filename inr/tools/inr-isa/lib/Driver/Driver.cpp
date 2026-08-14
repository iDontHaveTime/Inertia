// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/CLI/CTOpts.h>
#include <inr/Support/Stream.h>
#include <inr/Support/Version.h>
#include <inr/Vfs/FStream.h>
#include <inr/Vfs/Vfs.h>
#include <isa/Driver/Diag.h>
#include <isa/Driver/Driver.h>
#include <isa/Driver/Emitter.h>
#include <isa/Lexer/Lexer.h>
#include <isa/Parser/Parser.h>

#include <cstddef>
#include <memory>
#include <string_view>

namespace isa {

enum class ISAOpt {
    Input,
    StdinInput,
    Unknown,
#define NEW_ISA_OPT(IDENT, ...) IDENT,
#include "ISAOpts.inc"
#undef NEW_ISA_OPT
    LAST_OPT,
};

constexpr inr::CTOptsMap isaOpts = std::array{
#define NEW_ISA_OPT(IDENT, FLAGSTR, OPTKIND, HELPSTR)                       \
    inr::CTOptArg{FLAGSTR,                                                  \
                  inr::CTOpt{inr::CTOpt::OPTKIND, uint32_t(ISAOpt::IDENT)}, \
                  HELPSTR},
#include "ISAOpts.inc"
#undef NEW_ISA_OPT
};

void Driver::printHelp() {
    inr::out() << "Usage: inr-randir [options...]\nOptions:\n";
    isaOpts.printHelp(inr::out());
}

void Driver::printVersion() {
    inr::out() << "Inertia inr-isa version " << inr::getInertiaVersion()
               << '\n';
}

int Driver::isaMain(int argc, char** argv) {
    inr::CTOptParser<std::size_t(ISAOpt::LAST_OPT)> optsParser(
        isaOpts, argc, argv, '-', uint32_t(ISAOpt::Input),
        uint32_t(ISAOpt::StdinInput), uint32_t(ISAOpt::Unknown));

    if(optsParser.has(uint32_t(ISAOpt::Help))) {
        printHelp();
        return 0;
    }

    if(optsParser.has(uint32_t(ISAOpt::Version))) {
        printVersion();
        return 0;
    }

    if(optsParser.has(uint32_t(ISAOpt::PrintEmitters))) {
        auto e = Emitter::getAvailableEmitters();
        inr::out() << "Available emitters:\n";
        for(const auto& v : e) {
            inr::out() << v.name << '\n';
        }
        return 0;
    }

    if(optsParser.has(uint32_t(ISAOpt::HelpEmitter))) {
        std::string_view name =
            optsParser.getLast(uint32_t(ISAOpt::HelpEmitter)).value;
        auto e = Emitter::getByName(
            optsParser.getLast(uint32_t(ISAOpt::HelpEmitter)).value);
        if(!e) {
            printError("emitter '", name, "' not found");
            return 1;
        }
        inr::out() << e->helptext;
        return 0;
    }

    bool err = false;

    if(optsParser.has(uint32_t(ISAOpt::StdinInput))) {
        err = true;
        printError("stdin input is not supported");
    }

    if(optsParser.has(uint32_t(ISAOpt::Unknown))) {
        err = true;
        auto vec = optsParser.get(uint32_t(ISAOpt::Unknown));
        for(auto unknownOpt : vec) {
            printError("unknown opt '", unknownOpt->value, '\'');
        }
    }

    if(unsigned inputCount = optsParser.howMany(uint32_t(ISAOpt::Input))) {
        if(inputCount > 1) {
            err = true;
            printError("more than one input is present");
        }
    }
    else {
        err = true;
        printError("no input files");
    }

    if(err) return 1;

    auto& vfs = inr::vfs::getNativeFs();

    std::string_view inputFile =
        optsParser.getLast(uint32_t(ISAOpt::Input)).value;

    std::error_code ec;
    auto inputStat = vfs.stat(inputFile, ec);

    if(ec != std::error_code()) {
        printError("failed to stat file '", inputFile,
                   "' reason: ", ec.message());
        return 1;
    }

    switch(inputStat.getFT()) {
        case inr::vfs::FileType::None:
        case inr::vfs::FileType::NotFound:
            printError("file '", inputFile, "' not found");
            return 1;
        case inr::vfs::FileType::Regular:
            break;
        case inr::vfs::FileType::Directory:
            printError('\'', inputFile, "' is a directory");
            return 1;
        case inr::vfs::FileType::Symlink:
            break;
        case inr::vfs::FileType::CharacterDevice:
        case inr::vfs::FileType::FIFO:
        case inr::vfs::FileType::Socket:
            printError('\'', inputFile, "' is not a regular file");
            return 1;
    }

    std::size_t fileSize = inputStat.getSize();
    std::unique_ptr<char[]> fileMap(std::make_unique<char[]>(fileSize));

    auto file = vfs.open(inputFile, inr::vfs::OpenMode::OREAD, ec);
    if(ec != std::error_code()) {
        printError("failed to open file '", inputFile,
                   "' reason: ", ec.message());
        return 1;
    }

    if(std::size_t b = file->read(fileMap.get(), fileSize); b != fileSize) {
        printError("file size is ", fileSize, " but only ", b, " was read");
        return 1;
    }

    Lexer lexer(fileMap.get(), fileMap.get() + fileSize, inputFile);

    Diag diag;
    Parser parser(lexer, diag);

    auto expr = parser.parse();
    diag.printall(inr::log());
    if(auto info = diag.getInfo(); info.errors || info.fatals) {
        return 1;
    }
    inr::log().flush();

    if(optsParser.has(uint32_t(ISAOpt::PrintParsed))) {
        parser.printParsed(inr::out(), expr);
        return 0;
    }

    if(unsigned hm = optsParser.howMany(uint32_t(ISAOpt::ChooseEmitter))) {
        if(hm > 1) {
            printError("more than one emitter chosen");
            return 1;
        }

        const auto& emitterOpt =
            optsParser.getLast(uint32_t(ISAOpt::ChooseEmitter));
        auto emitter = Emitter::getByName(emitterOpt.value);

        if(!emitter) {
            printError("emitter '", emitterOpt.value, "' not found");
            return 1;
        }

        if(unsigned ofn = optsParser.howMany(uint32_t(ISAOpt::Output))) {
            if(ofn > 1) {
                printError("more than one output specified");
                return 1;
            }
        }
        else {
            printError("no output specified");
            return 1;
        }

        auto outputOpt = optsParser.getLast(uint32_t(ISAOpt::Output));
        auto outputFile = vfs.open(
            outputOpt.value,
            inr::vfs::OpenMode(inr::vfs::OWRITE | inr::vfs::OTRUNC), ec);
        if(ec != std::error_code{}) {
            printError("couldn't open file '", outputOpt.value,
                       "' with reason: ", ec.message());
            return 1;
        }

        inr::vfsstream fs(std::move(outputFile));
        if(optsParser.has(uint32_t(ISAOpt::IncludeLicense))) {
            fs << "// Copyright (c) 2026 Inertia Project\n// Distributed under "
                  "the Boost Software License, Version 1.0.\n// See LICENSE "
                  "file or https://www.boost.org/LICENSE_1_0.txt\n\n";
        }

        emitter->emitter->emit(fs, expr);
    }
    else {
        printError("no emitter chosen");
        return 1;
    }

    return 0;
}

} // namespace isa
