/// @file tools/inr-gen.cpp
/// @brief The inertia gen frontend tool.
///
/// This tool takes `.ing` (Inertia gen) files and generates C++ files to
/// include. Its mainly used for description of targets, such as their
/// instructions, endian, pointer size, etc..

#include <inr/ADT/StrView.h>
#include <inr/Gen/Lexer.h>
#include <inr/Option/ArgList.h>
#include <inr/Option/Option.h>
#include <inr/Option/OptionTable.h>
#include <inr/Support/Logger.h>
#include <inr/Support/Stream.h>

#include <cstdio>
#include <cstdlib>

constexpr inr::sview TOOL_NAME("inr-gen");

int main(int argc, char** argv) {
    /// Should be replaced with inr-gen generated opts.
    std::string input;
    std::string output;

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
                return 1;
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

    if(input.empty() || output.empty()) return 1;

    FILE* inputFile = fopen(input.c_str(), "r");

    fseek(inputFile, 0, SEEK_END);
    long inputFileSize = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    char* inputFileStart = (char*)malloc(inputFileSize);

    std::vector<inr::gen::token> tokens =
        inr::gen::lexer::lex(inputFileStart, inputFileStart + inputFileSize);

    free(inputFileStart);

    fclose(inputFile);

    return 0;
}