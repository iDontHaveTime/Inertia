// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Support/Unreachable.h>
#include <isa/Lexer/Lexer.h>

#include <string_view>

namespace isa {

struct LexerChars {
    enum CType : unsigned char {
        Whitespace,
        Alpha,
        Number,
        Symbol,
        Unknown,
    };
    CType charTypes[0x100];

    constexpr void classifyStr(std::string_view str, CType as) {
        for(char c : str) {
            charTypes[(unsigned char)c] = as;
        }
    }

    constexpr LexerChars() : charTypes() {
        std::fill(charTypes, charTypes + sizeof(charTypes), Unknown);
        classifyStr(" \t\n\v\r\f", Whitespace);
        classifyStr("ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz",
                    Alpha);
        classifyStr("0123456789", Number);
        classifyStr("!@#$%^&*()+=-`~/.,<>?';\":|[]{}", Symbol);
    }

    CType getType(char c) const {
        return charTypes[(unsigned char)c];
    }

    bool isWhitespace(char c) const {
        return getType(c) == Whitespace;
    }

    bool isAlpha(char c) const {
        return getType(c) == Alpha;
    }

    bool isNumber(char c) const {
        return getType(c) == Number;
    }

    bool isSymbol(char c) const {
        return getType(c) == Symbol;
    }

    bool isUnknown(char c) const {
        return getType(c) == Unknown;
    }
};

constexpr static LexerChars lchars;

void Lexer::skipWhitespace() {
    while(!isEof()) {
        if(!lchars.isWhitespace(currentChar())) return;
        advance();
    }
}

void Lexer::skipLine() {
    while(!isEof()) {
        if(currentChar() == '\n') {
            advance();
            return;
        }
        advance();
    }
}

Token Lexer::caseUnknown() {
    const char* start = getCur();
    while(!isEof()) {
        if(!lchars.isUnknown(currentChar())) break;
        advance();
    }
    return Token{{start, getCur()}, Token::UnknownSequence};
}

Token Lexer::caseAlpha() {
    const char* start = getCur();
    while(!isEof()) {
        if(auto t = lchars.getType(currentChar());
           t != LexerChars::Alpha && t != LexerChars::Number)
            break;
        advance();
    }
    return Token{{start, getCur()}, Token::Identifier};
}

Token Lexer::caseNumber() {
    const char* start = getCur();
    while(!isEof()) {
        if(!lchars.isNumber(currentChar())) break;
        advance();
    }
    return Token{{start, getCur()}, Token::NumberSequence};
}

Token Lexer::caseSymbol() {
    const char* start = getCur();
    char c = *start;
    advance();

    switch(c) {
        case '/':
            if(!isEof() && currentChar() == '/') {
                skipLine();
                return next();
            }
            break;
        case '(':
            return {{start, getCur()}, Token::LParen};
        case ')':
            return {{start, getCur()}, Token::RParen};
        case '<':
            return {{start, getCur()}, Token::LArrow};
        case '>':
            return {{start, getCur()}, Token::RArrow};
        case '"':
            return caseString();
        default:
            break;
    }

    return {{start, getCur()}, Token::UnknownSymbol};
}

Token Lexer::caseString() {
    const char* start = getCur();
    const char* end = start;
    bool escape = false;

    while(!isEof()) {
        char c = currentChar();

        if(c == '"' && !escape) {
            end = getCur();
            advance();
            break;
        }
        escape = false;

        if(c == '\\') {
            escape = true;
        }
        advance();
    }

    return {{start, end}, Token::String};
}

Token Lexer::next() {
    skipWhitespace();
    if(isEof()) return {{}, Token::EndOfFile};

    switch(lchars.getType(currentChar())) {
        case LexerChars::Alpha:
            return caseAlpha();
        case LexerChars::Number:
            return caseNumber();
        case LexerChars::Symbol:
            return caseSymbol();
        case LexerChars::Unknown:
            return caseUnknown();
        case LexerChars::Whitespace:
            inr_unreachable("Whitespace is skipped beforehand");
        default:
            inr_unreachable("This should not be possible");
    }
}

std::pair<unsigned long, unsigned long> Lexer::getLineAndColumn() const {
    unsigned long line = 1;
    unsigned long column = 1;

    for(const char* it = ctx_.start; it < ctx_.cur; it++) {
        column++;
        if(*it == '\n') {
            column = 0;
            line++;
        }
    }

    return {line, column};
}

} // namespace isa
