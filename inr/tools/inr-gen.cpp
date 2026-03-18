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
#include <inr/Gen/Lexer.h>
#include <inr/Gen/Parser.h>
#include <inr/Gen/Record.h>
#include <inr/Option/ArgList.h>
#include <inr/Option/Option.h>
#include <inr/Option/OptionTable.h>
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

    FILE* inputFile = fopen(input.c_str(), "r");
    if(!inputFile) {
        inr::log::send(inr::errs(), inr::log::Level::ERROR, TOOL_NAME,
                       "input file not found");
        return 1;
    }

    fseek(inputFile, 0, SEEK_END);
    long inputFileSize = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    inr::MemoryFile memInput(inputFile, inputFileSize, true);

    inr::gen::lexer genLex(input, memInput.begin(), memInput.end());

    std::list<inr::gen::token> tokens = genLex.lex();

    auto rec = inr::gen::parser::parseTokens(tokens);

    for(auto& node : rec->getNodes()) {
        if(node->getKind() == inr::gen::Node::NodeType::Target) {
            inr::gen::TargetNode* tn = (inr::gen::TargetNode*)node.get();
            inr::outs() << "Found target: " << tn->getName() << '\n'
                        << "Target endian: " << tn->getEndian() << '\n'
                        << "Target pointer width: " << tn->getPtrWidth()
                        << '\n';
        }
    }

    fclose(inputFile);

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

    if(output.empty()) {
        inr::log::send(inr::errs(), inr::log::Level::ERROR, TOOL_NAME,
                       "no output file");
    }

    if(input.empty()) {
        inr::log::send(inr::errs(), inr::log::Level::ERROR, TOOL_NAME,
                       "no input file");
    }

    if(input.empty() || output.empty()) return true;
    return false;
}