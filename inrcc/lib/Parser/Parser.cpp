// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inrcc/AST/Type.h>
#include <inrcc/Lexer/Lexer.h>
#include <inrcc/Parser/Parser.h>

namespace inrcc {

Parser::DeclContainer Parser::parseAll() {
    DeclContainer cont;

    for(consume(); curKind() != TokenKind::TOKEN_END; consume()) {
        parseTopDecl(cont);
    }

    return cont;
}

void Parser::parseTopDecl(DeclContainer& to) {}

} // namespace inrcc
