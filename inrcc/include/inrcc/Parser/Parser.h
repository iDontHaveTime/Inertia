// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_PARSER_PARSER_H
#define INRCC_PARSER_PARSER_H

/// @file Parser/Parser.h
/// @brief Contains a parser for C family languages.

#include <inr/ADT/IVector.h>
#include <inrcc/AST/DeclAST.h>
#include <inrcc/AST/Type.h>
#include <inrcc/Diagnostics/Diagnostics.h>
#include <inrcc/Lexer/Lexer.h>
#include <inrcc/Sema/Sema.h>

namespace inrcc {

class Parser {
    Lexer& lexer_;
    Arena& arena_;
    Sema& sema_;
    Diagnostics& diag_;
    Token cur_;

    void consume() {
        cur_ = lexer_.next();
    }

    TokenKind curKind() const noexcept {
        return cur_.getKind();
    }

    /// @brief Returns a diagnostic based on the span given.
    void newDiag(SourceLoc span, Diagnostics::Diag t) {
        diag_.newDiag(lexer_.getCurrentFile(), span.getPtr(), span.getLen(), t);
    }

    /// @brief Returns a diagnostic based on current token.
    void newDiag(Diagnostics::Diag t) {
        newDiag(cur_.getLoc(), t);
    }

public:
    Parser(Lexer& lexer, Arena& arena, Sema& sema, Diagnostics& diag) noexcept :
        lexer_(lexer), arena_(arena), sema_(sema), diag_(diag) {}

    using DeclContainer = inr::ivec<Decl*, 4>;

    DeclContainer parseAll();

    void parseTopDecl(DeclContainer&);
};

} // namespace inrcc

#endif // INRCC_PARSER_PARSER_H
