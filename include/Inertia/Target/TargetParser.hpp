#ifndef INERTIA_TARGETPARSER_HPP
#define INERTIA_TARGETPARSER_HPP

#include "Inertia/Lexer/LexerOutput.hpp"
#include "Inertia/Target/TargetFile.hpp"
#include "Inertia/Target/TargetOutput.hpp"

namespace Inertia{
    enum class TargetParserType{
        REGCLASS,
        REGISTER,
        DATAENT,
        DATAFIELD,
    };
    struct TargetParser{
        const TargetFile file;
        bool cpp_injections = false;

        TargetParser() = delete;
        TargetParser(const TargetFile& _file) noexcept : file(_file){};

        TargetOutput parse(std::vector<LexerOutput*> files);
    };
}

#endif // INERTIA_TARGETPARSER_HPP
