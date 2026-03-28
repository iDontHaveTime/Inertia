// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_GEN_PARSER_H
#define INERTIA_GEN_PARSER_H

/// @file Gen/Parser.h
/// @brief Contains the parser for inr-gen.

#include <inr/ADT/StrView.h>
#include <inr/Gen/Lexer.h>
#include <inr/Gen/Record.h>

namespace inr::gen {

class parser {
public:
    /// @brief Parses the provided tokens.
    /// @param tokens Result from the lexer.
    /// @param result Destination of the parser.
    static bool parse(class GenDriver& driver, const std::vector<token>& tokens,
                      RecordStorage& result);
};

} // namespace inr::gen

#endif // INERTIA_GEN_PARSER_H
