// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_PARSER_PARSER
#define INERTIA_PARSER_PARSER

/// @file Parser/Parser.h
/// @brief Provides the main parser class for the `inr-isa` tool.

#include <isa/Driver/Diag.h>
#include <isa/Lexer/Lexer.h>
#include <isa/Parser/Expression.h>

#include <memory>
#include <vector>

namespace isa {

class Parser {
    Lexer& lexer_;
    Diag& diag_;
    Token cur_;

    void consume() {
        cur_ = lexer_.next();
    }

public:
    Parser(Lexer& lexer, Diag& diag) : lexer_(lexer), diag_(diag) {}

    void newDiag(Diag::Severity sev, std::string_view span, std::string msg) {
        auto loc = lexer_.getLineAndColumn();
        diag_.newDiagnostic(sev, span, std::move(msg), loc.first, loc.second,
                            lexer_.currentFile());
    }

    bool parseExpr(std::vector<std::unique_ptr<Value>>& to);
    bool parseString(std::vector<std::unique_ptr<Value>>& to);
    bool parseTIR(std::vector<std::unique_ptr<Value>>& to);
    bool parseIdent(std::vector<std::unique_ptr<Value>>& to);
    bool parseAny(std::vector<std::unique_ptr<Value>>& to);

    std::vector<std::unique_ptr<Value>> parse();

    static void printParsed(inr::stream&,
                            const std::vector<std::unique_ptr<Value>>& exprs);
};

} // namespace isa

#endif // INERTIA_PARSER_PARSER
