// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_LEXER_LEXER
#define INERTIA_LEXER_LEXER

#include <inr/Support/Assert.h>

#include <string_view>
#include <vector>

namespace isa {

struct Token {
    std::string_view view;
    enum TokenID {
        Identifier,
        LParen,
        RParen,
        LArrow,
        RArrow,
        String,
        NumberSequence,
        UnknownSequence,
        UnknownSymbol,
        EndOfFile,
    } id;
};

struct LexerContext {
    const char *start, *cur, *end;
    std::string_view fileName;
};

class Lexer {
    std::vector<LexerContext> contextStack_;
    LexerContext ctx_;

    void skipWhitespace();
    void skipLine();

    Token caseUnknown();
    Token caseAlpha();
    Token caseNumber();
    Token caseSymbol();
    Token caseString();

    void advance() {
        ctx_.cur++;
    }

    const char* getCur() const {
        return ctx_.cur;
    }

    char currentChar() const {
        return *getCur();
    }

public:
    Lexer(const char* start, const char* end, std::string_view fileName) :
        ctx_(start, start, end, fileName) {}

    void pop_context() {
        inr_assert(contextStack_.size(),
                   "Lexer pop_context(): context vector is empty");
        ctx_ = contextStack_.back();
        contextStack_.pop_back();
    }

    void push_context(const char* start, const char* end,
                      std::string_view fileName) {
        contextStack_.emplace_back(ctx_);
        ctx_ = {start, start, end, fileName};
    }

    Token next();

    std::string_view currentFile() const {
        return ctx_.fileName;
    }

    bool isEof() const {
        return ctx_.cur == ctx_.end;
    }

    std::pair<unsigned long, unsigned long> getLineAndColumn() const;
};

} // namespace isa

#endif // INERTIA_LEXER_LEXER
