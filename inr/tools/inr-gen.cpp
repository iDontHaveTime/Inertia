// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

/// @file tools/inr-gen.cpp
/// @brief The inertia gen frontend tool.
///
/// This tool takes `.ing` (Inertia gen) files and generates C++ files to
/// include. Its mainly used for description of targets, such as their
/// instructions, endian, pointer size, etc..

#include <inr/ADT/StrView.h>
#include <inr/Gen/CppEmitter.h>
#include <inr/Gen/Lexer.h>
#include <inr/Gen/Parser.h>
#include <inr/Gen/Record.h>
#include <inr/Option/ArgList.h>
#include <inr/Option/Option.h>
#include <inr/Option/OptionTable.h>
#include <inr/Support/CFile.h>
#include <inr/Support/Logger.h>
#include <inr/Support/MemoryFile.h>
#include <inr/Support/Stream.h>

#include <cstdio>

constexpr inr::sview TOOL_NAME("inr-gen");

bool getInputAndOutputFiles(int argc, char** argv, std::string& input,
                            std::string& output);

int main(int argc, char** argv) {
    /// Should be replaced with inr-gen generated opts.
    std::string input;
    std::string output;

    if(getInputAndOutputFiles(argc, argv, input, output)) return 1;

    inr::CFile inputFile(input.c_str(), "r");
    if(!inputFile) {
        inr::log::send(inr::errs(), inr::log::Level::ERROR, TOOL_NAME,
                       "input file not found");
        return 1;
    }

    fseek(inputFile.getFile(), 0, SEEK_END);
    long inputFileSize = ftell(inputFile.getFile());
    fseek(inputFile.getFile(), 0, SEEK_SET);

    inr::MemoryFile memInput(inputFile.getFile(), inputFileSize, true);

    inr::gen::lexer genLex(input, memInput.begin(), memInput.end());

    std::vector<inr::gen::token> tokens = genLex.lex();

    std::vector<inr::gen::token> extTokens =
        inr::gen::parser::parseExtensions(tokens);
    if(extTokens.empty() && !tokens.empty()) {
        return 1;
    }

    auto rec = inr::gen::parser::parseTokens(extTokens);

    FILE* outputCFile = fopen(output.c_str(), "w");
    if(!outputCFile) {
        inr::log::send(inr::errs(), inr::log::Level::ERROR, TOOL_NAME,
                       "output could not be opened");
        return 1;
    }
    inr::standard_file_stream outputFile(outputCFile, true);

    inr::gen::emitter::emit(outputFile, rec);

    return 0;
}

bool getInputAndOutputFiles(int argc, char** argv, std::string& input,
                            std::string& output) {
    int i = 1;
    while(i < argc) {
        inr::sview arg = inr::sview(argv[i]);

        if(arg == "-o") {
            if(!output.empty()) break;
            if(i + 1 == argc) {
                inr::log::send(inr::errs(), inr::log::Level::ERROR, TOOL_NAME,
                               "argument to '-o' is missing");
                break;
            }
            else {
                output = std::string(argv[++i]);
            }
        }
        else {
            if(!input.empty()) {
                inr::log::send(inr::errs(), inr::log::Level::ERROR, TOOL_NAME,
                               "too many input files specified");
                return true;
            }
            else {
                input = std::move(arg.str());
            }
        }
        i++;
    }

    if(input.empty()) {
        inr::log::send(inr::errs(), inr::log::Level::ERROR, TOOL_NAME,
                       "no input file");
    }

    if(output.empty()) {
        inr::log::send(inr::errs(), inr::log::Level::ERROR, TOOL_NAME,
                       "no output file");
    }

    if(input.empty() || output.empty()) return true;
    return false;
}