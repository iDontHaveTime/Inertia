// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_GEN_PARSER_H
#define INERTIA_GEN_PARSER_H

/// @file Gen/Parser.h
/// @brief Contains the parser for inr-gen.

#include <inr/Gen/Lexer.h>
#include <inr/Gen/Record.h>

#include <memory>

namespace inr::gen {

class parser {
public:
    /// @brief Parses inr-gen extensions such as unfolding.
    /// @param tokens The tokens to parse.
    /// @return New token array after extensions.
    static std::vector<token> parseExtensions(const std::vector<token>& tokens);
    /// @brief Parses tokens and returns the file tree.
    /// @param tokens The tokens to parse.
    /// @return Root node for the records.
    static std::unique_ptr<Node> parseTokens(const std::vector<token>& tokens);
};

} // namespace inr::gen

#endif // INERTIA_GEN_PARSER_H
