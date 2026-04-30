// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/ADT/ArrView.h>
#include <inr/ADT/IVector.h>
#include <inr/ADT/StrView.h>
#include <inr/Support/Compiler.h>
#include <inrcc/Lexer/Lexer.h>

#include <cstdint>

#define inrcc_inline inr_useattr(always_inline)
#define inrcc_hot inr_useattr(hot)

namespace inrcc {

enum LexerLookup : uint8_t {
    LNone = 0,
    LWhitespace = 0x1,
    LAlpha = 0x2,
    LNum = 0x4,
    LSymbol = 0x8,
    LErr = 0x10,
    LTerm = 0x20,
    LNL = 0x40
};

enum LexerNumber : uint8_t {
    LNumNone = 0,
    LHexadecimal = 0x1,
    LOctal = 0x2,
    LBinary = 0x4
};

class LexerLookupTable {
    uint8_t table_[0x100];
    uint8_t tokenKind_[0x80];
    uint8_t numberKind_[0x80];

public:
    constexpr LexerLookupTable() : table_(), tokenKind_(), numberKind_() {
        for(char c : inr::sview(
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz")) {
            table_[(unsigned char)c] = LAlpha;
        }
        for(char c : inr::sview("0123456789")) {
            table_[(unsigned char)c] = LNum;
        }
        for(char c : inr::sview("!\"#%&'()*+,-./:;<=>?[\\]^{|}~")) {
            table_[(unsigned char)c] = LSymbol;
        }
        // Whitespaces are:
        // 0x20 - 0b00100000
        // 0x09 - 0b00001001
        // 0x0A - 0b00001010
        // 0x0B - 0b00001011
        // 0x0C - 0b00001100
        // 0x0D - 0b00001101
        for(char c : inr::sview("\x20\x09\x0A\x0B\x0C\x0D")) {
            table_[(unsigned char)c] = LWhitespace;
        }
        for(unsigned i = 1; i < 0x100; i++) {
            if(!table_[i]) table_[i] = LErr;
        }
        table_[0] = LTerm;
        table_[(unsigned char)'\n'] |= LNL;

        tokenKind_[(unsigned char)'!'] = (uint8_t)TokenKind::SYMBOL_EXCLAMATION;
        tokenKind_[(unsigned char)'%'] = (uint8_t)TokenKind::SYMBOL_PERCENT;
        tokenKind_[(unsigned char)'&'] = (uint8_t)TokenKind::SYMBOL_AMPERSAND;
        tokenKind_[(unsigned char)'('] = (uint8_t)TokenKind::SYMBOL_LPAREN;
        tokenKind_[(unsigned char)')'] = (uint8_t)TokenKind::SYMBOL_RPAREN;
        tokenKind_[(unsigned char)'*'] = (uint8_t)TokenKind::SYMBOL_STAR;
        tokenKind_[(unsigned char)'+'] = (uint8_t)TokenKind::SYMBOL_PLUS;
        tokenKind_[(unsigned char)','] = (uint8_t)TokenKind::SYMBOL_COMMA;
        tokenKind_[(unsigned char)'-'] = (uint8_t)TokenKind::SYMBOL_MINUS;
        tokenKind_[(unsigned char)'.'] = (uint8_t)TokenKind::SYMBOL_DOT;
        tokenKind_[(unsigned char)'/'] = (uint8_t)TokenKind::SYMBOL_SLASH;
        tokenKind_[(unsigned char)':'] = (uint8_t)TokenKind::SYMBOL_COLON;
        tokenKind_[(unsigned char)';'] = (uint8_t)TokenKind::SYMBOL_SEMICOLON;
        tokenKind_[(unsigned char)'<'] = (uint8_t)TokenKind::SYMBOL_LESS;
        tokenKind_[(unsigned char)'='] = (uint8_t)TokenKind::SYMBOL_EQUAL;
        tokenKind_[(unsigned char)'>'] = (uint8_t)TokenKind::SYMBOL_GREATER;
        tokenKind_[(unsigned char)'?'] = (uint8_t)TokenKind::SYMBOL_QUESTION;
        tokenKind_[(unsigned char)'['] = (uint8_t)TokenKind::SYMBOL_LSQUARE;
        tokenKind_[(unsigned char)']'] = (uint8_t)TokenKind::SYMBOL_RSQUARE;
        tokenKind_[(unsigned char)'^'] = (uint8_t)TokenKind::SYMBOL_CARET;
        tokenKind_[(unsigned char)'{'] = (uint8_t)TokenKind::SYMBOL_LBRACE;
        tokenKind_[(unsigned char)'|'] = (uint8_t)TokenKind::SYMBOL_PIPE;
        tokenKind_[(unsigned char)'}'] = (uint8_t)TokenKind::SYMBOL_RBRACE;
        tokenKind_[(unsigned char)'~'] = (uint8_t)TokenKind::SYMBOL_TILDE;
        tokenKind_[(unsigned char)'#'] = (uint8_t)TokenKind::PREPROCESS_HASH;

        numberKind_[(unsigned char)'0'] = LBinary | LOctal | LHexadecimal;
        numberKind_[(unsigned char)'1'] = LBinary | LOctal | LHexadecimal;
        numberKind_[(unsigned char)'2'] = LOctal | LHexadecimal;
        numberKind_[(unsigned char)'3'] = LOctal | LHexadecimal;
        numberKind_[(unsigned char)'4'] = LOctal | LHexadecimal;
        numberKind_[(unsigned char)'5'] = LOctal | LHexadecimal;
        numberKind_[(unsigned char)'6'] = LOctal | LHexadecimal;
        numberKind_[(unsigned char)'7'] = LOctal | LHexadecimal;
        numberKind_[(unsigned char)'8'] = LHexadecimal;
        numberKind_[(unsigned char)'9'] = LHexadecimal;
        numberKind_[(unsigned char)'A'] = LHexadecimal;
        numberKind_[(unsigned char)'B'] = LHexadecimal;
        numberKind_[(unsigned char)'C'] = LHexadecimal;
        numberKind_[(unsigned char)'D'] = LHexadecimal;
        numberKind_[(unsigned char)'E'] = LHexadecimal;
        numberKind_[(unsigned char)'F'] = LHexadecimal;
    }

    inrcc_hot inrcc_inline constexpr inline uint8_t lookup(
        char c) const noexcept {
        return table_[(unsigned char)c];
    }

    inrcc_inline constexpr inline uint8_t fastTokenKind(char c) const noexcept {
        return tokenKind_[(unsigned char)c];
    }

    inrcc_inline constexpr inline uint8_t fastNumberKind(
        char c) const noexcept {
        return numberKind_[(unsigned char)c];
    }

} constexpr static lookupTable{};

inrcc_inline void Lexer::skipWhitespace() {
    while(char c = *ptr_) {
        if(!(lookupTable.lookup(c) & LWhitespace)) return;
        skip();
    }
}

inrcc_inline bool Lexer::skipWhitespacePreProcess() {
    while(char c = *ptr_) {
        uint8_t kind = lookupTable.lookup(c);
        if(kind & ~LWhitespace) {
            if(kind & LNL) return true;
            return false;
        }
        skip();
    }
    return false;
}

bool Lexer::caseNone(Token& tok) {
    if(includeStack_.size()) {
        popFilePtr();
        return true;
    }
    tok.setLength(0);
    tok.setKind(TokenKind::TOKEN_END);
    return false;
}

void Lexer::caseAlpha(Token& tok) {
    while(char c = *ptr_) {
        uint8_t kind = lookupTable.lookup(c);

        if(!(kind & (LAlpha | LNum))) {
            tok.setLength(ptr_ - tok.getStart());
            inr::sview view = tok.getView();
            IdentInfo** info = infoTable_.find(view);
            if(!info) {
                tok.setKind(TokenKind::TOKEN_IDENT);
                tok.setIdentInfo(*infoTable_.insert(
                    view,
                    arena_.alloc<IdentInfo>(TokenKind::TOKEN_IDENT,
                                            tok.getStart(), tok.getLength())));
            }
            else {
                tok.setIdentInfo(*info);
                tok.setKind((*info)->getKind());
            }
            return;
        }

        skip();
    }
    return;
}

void Lexer::caseNum(Token& tok) {
    tok.setKind(TokenKind::LITERAL_INTEGER);
    if(forward('0')) {
        char c = *ptr_;
        switch(c) {
            case 'b':
            case 'B':
                break;
            case 'x':
            case 'X':
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
                break;
            default:
                tok.setLength(1);
                return;
        }
    }
    else {
        while(char c = *ptr_) {
            if(!(lookupTable.lookup(c) & LNum)) {
                tok.setLength(ptr_ - tok.getStart());
                return;
            }
            skip();
        }
    }
}

void Lexer::handleInclude() {
    if(skipWhitespacePreProcess()) return;

    Token tok = nextNoPreProcessingDirective();

    if(tok.getKind() == TokenKind::SYMBOL_LESS) {
        const char* start = ptr_;

        while(true) {
            if(skipWhitespacePreProcess())
                return; // ERROR. expected '>' to close include
            tok = nextNoPreProcessingDirective();
            if(tok.getKind() == TokenKind::SYMBOL_GREATER) {
                DriverFMan::File f = fman_.openFileBufferNoLocalDir(
                    inr::sview(start, tok.getStart()));

                skipLine();

                if(f.file) {
                    pushFilePtr(f.file->memfile.data(), f);
                }
                // else ERROR. file not found
                return;
            }
            else if(tok.getKind() == TokenKind::TOKEN_END)
                return; // ERROR. expected '>' to close include
        }
    }
    else if(tok.getKind() == TokenKind::LITERAL_STRING) {
    }
    else {
        skipLine();
        // ERROR. include must be <...> or "..."
    }
}

static inline bool isDistanceZero(const Token& t1, const Token& t2) {
    const char* it(t1.getStart() + t1.getLength());
    const char* itEnd(t2.getStart());

    while(*it == '\\' && *(it + 1) == '\n') {
        it += 2;
    }

    return *it == *itEnd;
}

void Lexer::handleDefine() {
    if(skipWhitespacePreProcess()) return;

    Token identTok;
    if(nextNoPreProcessing(identTok)) return;

    if(!identTok.getInfo()) {
        skipLine();
        return; // ERROR. expected an identifier
    }

    MacroInfo* minfo = arena_.alloc<MacroInfo>();
    macros_.insert(identTok.getStart(), identTok.getLength(), minfo);

    Token tok;
    if(nextNoPreProcessing(tok)) return;

    if(tok.getKind() == TokenKind::SYMBOL_LPAREN &&
       isDistanceZero(identTok, tok)) {
        minfo->enableFunctionLike();
        bool lastWasComma = true;
        while(true) {
            if(nextNoPreProcessing(tok)) return;

            TokenKind kind = tok.getKind();

            if(kind == TokenKind::SYMBOL_RPAREN) {
                break;
            }

            if(kind == TokenKind::SYMBOL_ELLIPSIS) {
                minfo->enableVararg();
                if(!lastWasComma && !minfo->getArgs().empty()) {
                    minfo->setVarargIdent(minfo->getArgs().back());
                    minfo->getArgs().pop_back();
                }
                else {
                    minfo->setVarargIdent(
                        MacroInfo::getDefaultMacroIdentVararg());
                }

                if(nextNoPreProcessing(tok)) return;
                if(tok.getKind() != TokenKind::SYMBOL_RPAREN) {
                    skipLine();
                    return;
                }
                break;
            }

            if(kind == TokenKind::SYMBOL_COMMA) {
                if(lastWasComma) return; // ERROR. can't have MAC(,) or MAC(a,,)
                lastWasComma = true;
                continue;
            }

            if(IdentInfo* info = tok.getInfo()) {
                if(!lastWasComma)
                    return; // ERROR. identifier needs to come after a comma (or
                            // lparen)
                minfo->getArgs().emplace_back(info);
                lastWasComma = false;
            }
            else return; // ERROR. expected identifier
        }
    }
    else {
        minfo->newToken() = tok;
    }

    while(true) {
        if(nextNoPreProcessing(tok)) return;
        if(tok.getKind() == TokenKind::TOKEN_END) return;
        minfo->newToken() = tok;
    }
}

void Lexer::handleUndef() {
    if(skipWhitespacePreProcess()) return;

    uint8_t kind = lookupTable.lookup(*ptr_);

    if(!(kind & LAlpha)) {
        skipLine();
        // ERROR.
        return;
    }

    Token tok;
    if(nextNoPreProcessing(tok)) return;
    macros_.eraseIfFound(tok.getStart(), tok.getLength());

    skipLine();
}

void Lexer::handleIfDef(bool invert) {
    if(skipWhitespacePreProcess()) return;

    uint8_t kind = lookupTable.lookup(*ptr_);

    if(!(kind & LAlpha)) {
        skipLine();
        // ERROR.
        return;
    }

    Token tok;
    if(nextNoPreProcessing(tok)) return;

    conditionsStack_.push_back(
        (macros_.find(tok.getStart(), tok.getLength()) != nullptr) ^ invert);

    skipLine();
}

void Lexer::handlePreprocessing() {
    if(skipWhitespacePreProcess()) return;

    uint8_t kind = lookupTable.lookup(*ptr_);

    if(!(kind & LAlpha)) {
        skipLine();
        // ERROR.
        return;
    }

    Token tok;
    tok.setStart(ptr_);

    caseAlpha(tok);

    switch(tok.getKind()) {
        case TokenKind::KEYWORD_define:
            if(!tokensAllowed()) {
                skipLine();
                return;
            }
            handleDefine();
            break;
        case TokenKind::KEYWORD_include:
            if(!tokensAllowed()) {
                skipLine();
                return;
            }
            handleInclude();
            break;
        case TokenKind::KEYWORD_include_next:
            if(!tokensAllowed()) {
                skipLine();
                return;
            }
            // TODO: Handle include next
            break;
        case TokenKind::KEYWORD_elif:
            if(skipElse()) {
                skipLine();
                return;
            }
            if(!conditionsStack_.empty()) {
                bool& res = conditionsStack_.back();
                if(!res) {
                    int exprRes = parseIfExpr();
                    if(exprRes != -1) {
                        res = exprRes;
                    }
                    // else ERROR. failed to evaluate
                }
            }
            break;
        case TokenKind::KEYWORD_if: {
            if(!tokensAllowed()) {
                skipLine();
                conditionsStack_.emplace_back(false);
                return;
            }
            int exprRes = parseIfExpr();
            if(exprRes != -1) conditionsStack_.emplace_back(exprRes);
            else
                conditionsStack_.emplace_back(
                    false); // ERROR. failed to evaluate
        } break;
        case TokenKind::KEYWORD_else:
            if(skipElse()) {
                skipLine();
                return;
            }
            skipLine();
            if(!conditionsStack_.empty()) {
                bool& res = conditionsStack_.back();
                res = !res;
            }
            break;
        case TokenKind::KEYWORD_ifdef:
            if(!tokensAllowed()) {
                skipLine();
                conditionsStack_.emplace_back(false);
                return;
            }
            handleIfDef(false);
            break;
        case TokenKind::KEYWORD_ifndef:
            if(!tokensAllowed()) {
                skipLine();
                conditionsStack_.emplace_back(false);
                return;
            }
            handleIfDef(true);
            break;
        case TokenKind::KEYWORD_undef:
            if(!tokensAllowed()) {
                skipLine();
                return;
            }
            handleUndef();
            break;
        case TokenKind::KEYWORD_endif:
            skipLine();
            if(!conditionsStack_.empty()) {
                conditionsStack_.pop_back();
            }
            // else ERROR.
            break;
        default:
            skipLine();
            // ERROR.
            return;
    }
}

void Lexer::caseString(Token& tok) {
    tok.setKind(TokenKind::LITERAL_STRING);
    tok.setStart(ptr_);

    while(true) {
        uint8_t kind = lookupTable.lookup(*ptr_);

        if(kind & LSymbol) {
            char c = *ptr_;
            if(c == '\\') {
                if(jump('"')) continue;
                if(jump('a')) continue;
                if(jump('b')) continue;
                if(jump('e')) continue;
                if(jump('f')) continue;
                if(jump('n')) continue;
                if(jump('r')) continue;
                if(jump('t')) continue;
                if(jump('v')) continue;
                if(jump('\\')) continue;
                if(jump('\'')) continue;
                if(jump('?')) continue;
                // TODO: Implement \x, \u, \U for escapes.
            }
            else if(c == '"') {
                tok.setLength(ptr_ - tok.getStart());
                skip();
                return;
            }
        }
        else if(kind & LNL) {
            return; // ERROR. string not closed
        }
        else if(kind & (LErr | LTerm)) {
            return;
        }
        skip();
    }
}

bool Lexer::caseSymbol(Token& tok, bool preprocess) {
    char c = *ptr_;
    skip();
    tok.setKind((TokenKind)lookupTable.fastTokenKind(c));

    switch(c) {
        case '"':
            caseString(tok);
            return false;
            break;
        case '#':
            if(preprocess) {
                handlePreprocessing();
                return true;
            }
            else if(forward('#')) {
                tok.setKind(TokenKind::PREPROCESS_HASHHASH);
            }
            break;
        case '%':
            if(forward('=')) {
                tok.setKind(TokenKind::SYMBOL_PERCENTEQUAL);
            }
            else if(forward('>')) {
                tok.setKind(TokenKind::SYMBOL_RBRACE);
            }
            else if(forward(':')) {
                if(preprocess) {
                    handlePreprocessing();
                    return true;
                }
                else if(forward('#')) {
                    tok.setKind(TokenKind::PREPROCESS_HASHHASH);
                    break;
                }
                // What about %:%:
                tok.setKind(TokenKind::PREPROCESS_HASH);
            }
            break;
        case '&':
            if(forward('=')) {
                tok.setKind(TokenKind::SYMBOL_AMPERSANDEQUAL);
            }
            else if(forward('&')) {
                tok.setKind(TokenKind::SYMBOL_AND);
            }
            break;
        case '\'':
            break;
        case '*':
            if(forward('=')) {
                tok.setKind(TokenKind::SYMBOL_STAREQUAL);
            }
            break;
        case '+':
            if(forward('=')) {
                tok.setKind(TokenKind::SYMBOL_PLUSEQUAL);
            }
            else if(forward('+')) {
                tok.setKind(TokenKind::SYMBOL_PLUSPLUS);
            }
            break;
        case '-':
            if(forward('=')) {
                tok.setKind(TokenKind::SYMBOL_MINUSEQUAL);
            }
            else if(forward('-')) {
                tok.setKind(TokenKind::SYMBOL_MINUSMINUS);
            }
            else if(forward('>')) {
                tok.setKind(TokenKind::SYMBOL_ARROW);
            }
            break;
        case '.':
            if(*ptr_ == '.') {
                if(jump('.')) {
                    tok.setKind(TokenKind::SYMBOL_ELLIPSIS);
                }
            }
            break;
        case '/':
            if(forward('/')) {
                skipLine(); // Since C99
                return true;
            }
            else if(forward('*')) {
                skipMultiline();
                return true;
            }
            else if(forward('=')) {
                tok.setKind(TokenKind::SYMBOL_SLASHEQUAL);
            }
            break;
        case '<':
            if(forward('=')) {
                if(lang_.getSpaceship() && forward('>')) {
                    tok.setKind(TokenKind::SYMBOL_SPACESHIP);
                }
                else tok.setKind(TokenKind::SYMBOL_LESSEQUAL);
            }
            else if(forward('<')) {
                if(forward('=')) {
                    tok.setKind(TokenKind::SYMBOL_SHIFTLEFTEQUAL);
                }
                else tok.setKind(TokenKind::SYMBOL_SHIFTLEFT);
            }
            else if(forward(':')) {
                tok.setKind(TokenKind::SYMBOL_LSQUARE);
            }
            else if(forward('%')) {
                tok.setKind(TokenKind::SYMBOL_LBRACE);
            }
            break;
        case '=':
            if(forward('=')) {
                tok.setKind(TokenKind::SYMBOL_EQUALEQUAL);
            }
            break;
        case '>':
            if(forward('=')) {
                tok.setKind(TokenKind::SYMBOL_GREATEREQUAL);
            }
            else if(forward('<')) {
                if(forward('=')) {
                    tok.setKind(TokenKind::SYMBOL_SHIFTRIGHTEQUAL);
                }
                else tok.setKind(TokenKind::SYMBOL_SHIFTRIGHT);
            }
            break;
        case '^':
            if(forward('=')) {
                tok.setKind(TokenKind::SYMBOL_CARETEQUAL);
            }
            break;
        case '|':
            if(forward('=')) {
                tok.setKind(TokenKind::SYMBOL_PIPEEQUAL);
            }
            else if(forward('|')) {
                tok.setKind(TokenKind::SYMBOL_OR);
            }
            break;
        case '!':
            if(forward('=')) {
                tok.setKind(TokenKind::SYMBOL_NOTEQUAL);
            }
            break;
        case ':':
            if(forward('>')) {
                tok.setKind(TokenKind::SYMBOL_RSQUARE);
            }
            break;
    }

    tok.setLength(ptr_ - tok.getStart());

    return false;
}

class ExprInteger {
    uint64_t value_;
    bool unsign_;

public:
    ExprInteger() noexcept : value_(0), unsign_(0) {}

    ExprInteger(uint64_t value, bool unsign) noexcept :
        value_(value), unsign_(unsign) {}

    ExprInteger(bool b) noexcept : ExprInteger(b, false) {}

    void setValue(uint64_t v) noexcept {
        value_ = v;
    }

    uint64_t getValue() const noexcept {
        return value_;
    }

    ExprInteger& operator+=(ExprInteger other) noexcept {
        unsign_ = unsign_ || other.unsign_;
        value_ += other.value_;
        return *this;
    }

    ExprInteger operator+(ExprInteger other) const noexcept {
        return ExprInteger(value_, unsign_) += other;
    }

    ExprInteger& operator-=(ExprInteger other) noexcept {
        unsign_ = unsign_ || other.unsign_;
        value_ -= other.value_;
        return *this;
    }

    ExprInteger operator-(ExprInteger other) const noexcept {
        return ExprInteger(value_, unsign_) -= other;
    }

    ExprInteger& operator*=(ExprInteger other) noexcept {
        unsign_ = unsign_ || other.unsign_;
        value_ *= other.value_;
        return *this;
    }

    ExprInteger operator*(ExprInteger other) const noexcept {
        return ExprInteger(value_, unsign_) *= other;
    }

    ExprInteger& operator/=(ExprInteger other) noexcept {
        unsign_ = unsign_ || other.unsign_;
        if(unsign_) {
            value_ /= other.value_;
        }
        else {
            value_ = (int64_t)value_ / (int64_t)other.value_;
        }
        return *this;
    }

    ExprInteger operator/(ExprInteger other) const noexcept {
        return ExprInteger(value_, unsign_) /= other;
    }

    ExprInteger& operator%=(ExprInteger other) noexcept {
        unsign_ = unsign_ || other.unsign_;
        if(unsign_) {
            value_ %= other.value_;
        }
        else {
            value_ = (int64_t)value_ % (int64_t)other.value_;
        }
        return *this;
    }

    ExprInteger operator%(ExprInteger other) const noexcept {
        return ExprInteger(value_, unsign_) %= other;
    }

    ExprInteger operator>(ExprInteger other) const noexcept {
        bool unsign = unsign_ || other.unsign_;
        bool val;

        if(unsign) {
            val = value_ > other.value_;
        }
        else {
            val = (int64_t)value_ > (int64_t)other.value_;
        }

        return ExprInteger(val, false);
    }

    ExprInteger operator<(ExprInteger other) const noexcept {
        bool unsign = unsign_ || other.unsign_;
        bool val;

        if(unsign) {
            val = value_ < other.value_;
        }
        else {
            val = (int64_t)value_ < (int64_t)other.value_;
        }

        return ExprInteger(val, false);
    }

    ExprInteger operator==(ExprInteger other) const noexcept {
        return ExprInteger(value_ == other.value_, false);
    }

    ExprInteger operator>=(ExprInteger other) const noexcept {
        bool unsign = unsign_ || other.unsign_;
        bool val;

        if(unsign) {
            val = value_ >= other.value_;
        }
        else {
            val = (int64_t)value_ >= (int64_t)other.value_;
        }

        return ExprInteger(val, false);
    }

    ExprInteger operator<=(ExprInteger other) const noexcept {
        bool unsign = unsign_ || other.unsign_;
        bool val;

        if(unsign) {
            val = value_ <= other.value_;
        }
        else {
            val = (int64_t)value_ <= (int64_t)other.value_;
        }

        return ExprInteger(val, false);
    }

    ExprInteger operator!=(ExprInteger other) const noexcept {
        return ExprInteger(value_ != other.value_, false);
    }

    ExprInteger& operator<<=(ExprInteger other) noexcept {
        value_ <<= other.value_;
        return *this;
    }

    ExprInteger operator<<(ExprInteger other) const noexcept {
        return ExprInteger(value_, unsign_) <<= other;
    }

    ExprInteger& operator>>=(ExprInteger other) noexcept {
        if(unsign_) {
            value_ >>= other.value_;
        }
        else {
            value_ = (int64_t)value_ >> (int64_t)other.value_;
        }
        return *this;
    }

    ExprInteger operator>>(ExprInteger other) const noexcept {
        return ExprInteger(value_, unsign_) >>= other;
    }

    ExprInteger& operator|=(ExprInteger other) noexcept {
        unsign_ = unsign_ || other.unsign_;
        value_ |= other.value_;
        return *this;
    }

    ExprInteger operator|(ExprInteger other) const noexcept {
        return ExprInteger(value_, unsign_) |= other;
    }

    ExprInteger& operator&=(ExprInteger other) noexcept {
        unsign_ = unsign_ || other.unsign_;
        value_ &= other.value_;
        return *this;
    }

    ExprInteger operator&(ExprInteger other) const noexcept {
        return ExprInteger(value_, unsign_) &= other;
    }

    ExprInteger& operator^=(ExprInteger other) noexcept {
        unsign_ = unsign_ || other.unsign_;
        value_ ^= other.value_;
        return *this;
    }

    ExprInteger operator^(ExprInteger other) const noexcept {
        return ExprInteger(value_, unsign_) ^= other;
    }

    ExprInteger operator||(ExprInteger other) const noexcept {
        return ExprInteger(value_ || other.value_, false);
    }

    ExprInteger operator&&(ExprInteger other) const noexcept {
        return ExprInteger(value_ && other.value_, false);
    }

    ExprInteger operator!() const noexcept {
        return ExprInteger(!value_, false);
    }

    ExprInteger operator-() const noexcept {
        return ExprInteger(unsign_ ? -value_ : -int64_t(value_), unsign_);
    }

    ExprInteger operator+() const noexcept {
        return *this;
    }

    ExprInteger operator~() const noexcept {
        return ExprInteger(~value_, unsign_);
    }
};

void Lexer::skipMultiline() {
    while(char c = *ptr_) {
        if(c == '*') {
            if(jump('/')) return;
        }
        skip();
    }
}

void Lexer::skipLine() {
    while(char c = *ptr_) {
        uint8_t kind = lookupTable.lookup(c);
        if(kind & LNL) {
            return;
        }

        skip();
    }
}

struct StringifyInfo {
    inr::sview range;
};

static inline void stringifyToken(StringifyInfo info, Arena& arena,
                                  Lexer::LexerTokenStack& to) {
    char* ptr = arena.allocN<char>(info.range.size());
    Token tok;
    tok.setStart(ptr);
    TokenCharIterator tokIt = info.range.begin();
    TokenCharIterator tokEnd = info.range.end();

    uint8_t lastC = LNone;
    while(tokIt != tokEnd) {
        uint8_t c = lookupTable.lookup(*tokIt);
        if(c & LWhitespace) {
            if(!(lastC & LWhitespace)) {
                *ptr = ' ';
                ptr++;
            }
        }
        else {
            *ptr = *tokIt;
            ptr++;
        }

        lastC = c;
        ++tokIt;
    }

    tok.setKind(TokenKind::LITERAL_STRING);
    tok.setLength(ptr - tok.getStart());
    to.emplace_back(arena.alloc<Token>(tok));
}

static inline StringifyInfo getTokenStringRange(const Token& start,
                                                const Token& end) {
    StringifyInfo info;

    info.range = {start.getStart(), end.getStart() + end.getLength()};

    if(start.getKind() == TokenKind::LITERAL_STRING)
        info.range = {info.range.begin() - 1, info.range.end()};
    if(end.getKind() == TokenKind::LITERAL_STRING)
        info.range = {info.range.begin(), info.range.end() + 1};

    return info;
}

bool Lexer::preprocess(const Token& tok, LexerTokenStack& to) {
    if(IdentInfo* info = tok.getInfo()) {
        auto minfo = macros_.find(info->getStart(), info->getLength());

        if(!minfo) return false;

        const MacroInfo* minfoEntry = *minfo;

        inr::ivec<inr::ivec<Token, 4>, 4> args;
        bool argsmatch = true;

        if(minfoEntry->isFunctionLike()) {
            if(minfoEntry->getArgs().size() || minfoEntry->isVararg())
                args.emplace_back();
            Token tok = nextNoPreProcessing();
            if(tok.getKind() != TokenKind::SYMBOL_LPAREN) {
                return false;
            }

            unsigned parenDepth = 0;
            while(true) {
                tok = nextNoPreProcessing();
                TokenKind kind = tok.getKind();

                if(kind == TokenKind::SYMBOL_LPAREN) {
                    parenDepth++;
                }
                else if(kind == TokenKind::SYMBOL_RPAREN) {
                    if(parenDepth == 0) break;
                    parenDepth--;
                }
                else if(kind == TokenKind::SYMBOL_COMMA && !parenDepth) {
                    if(args.size() < minfoEntry->getArgs().size()) {
                        args.emplace_back();
                        continue;
                    }
                    if(minfoEntry->isVararg() &&
                       args.size() == minfoEntry->getArgs().size()) {
                        args.emplace_back();
                        continue;
                    }
                }
                else if(kind == TokenKind::TOKEN_END)
                    return false; // ERROR. macro abruptly cut off

                if(!args.empty()) args.back().emplace_back(tok);
            }
        }

        if(!argsmatch || args.size() < minfoEntry->getArgs().size()) {
            // ERROR. sizes must match
            return false;
        }

        if(hideSet_.bfind(*minfo)) return false;
        hideSet_.emplace_back(minfoEntry);

        const auto& replacements = minfoEntry->getReplacements();
        for(auto revIt = replacements.rbegin(); revIt != replacements.rend();
            ++revIt) {
            const Token& reptok = *revIt;

            if(IdentInfo* tokInfo = reptok.getInfo()) {
                if(minfoEntry->isFunctionLike()) {
                    if(tokInfo == minfoEntry->getVarargIdent()) {
                        if(auto hashIt = revIt + 1;
                           hashIt != replacements.rend()) {
                            if(hashIt->getKind() ==
                               TokenKind::PREPROCESS_HASH) {
                                stringifyToken(
                                    getTokenStringRange(args.back().front(),
                                                        args.back().back()),
                                    arena_, to);
                                revIt = hashIt;
                                continue;
                            }
                        }

                        for(auto it = args.back().rbegin();
                            it != args.back().rend(); ++it) {
                            if(!preprocess(*it, to)) {
                                to.emplace_back(arena_.alloc<Token>(*it));
                            }
                        }

                        continue;
                    }

                    if(auto it = minfoEntry->getArgs().find(tokInfo);
                       it != minfoEntry->getArgs().end()) {
                        const auto& argVec =
                            args[it - minfoEntry->getArgs().begin()];

                        if(auto hashIt = revIt + 1;
                           hashIt != replacements.rend()) {
                            if(hashIt->getKind() ==
                               TokenKind::PREPROCESS_HASH) {
                                stringifyToken(
                                    getTokenStringRange(argVec.front(),
                                                        argVec.back()),
                                    arena_, to);
                                revIt = hashIt;
                                continue;
                            }
                        }

                        for(auto it = argVec.rbegin(); it != argVec.rend();
                            ++it) {
                            if(!preprocess(*it, to)) {
                                to.emplace_back(arena_.alloc<Token>(*it));
                            }
                        }

                        continue;
                    }
                }

                if(!preprocess(reptok, to)) {
                    to.emplace_back(&reptok);
                }
            }
            else {
                to.emplace_back(&reptok); // Non identifiers cannot be expanded.
            }
        }

        hideSet_.pop_back();
        return true;
    }

    return false;
}

class CPPParser {
    static inline ExprInteger fromToken(const char* start, uint32_t len) {
        TokenCharIterator it = start;
        TokenCharIterator itEnd = start + len;

        int base = 10;
        if(len > 1) {
            if(*it == '0') {
                ++it;
                switch(*it) {
                    case 'x':
                    case 'X':
                        base = 16;
                        ++it;
                        break;
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                        base = 8;
                        break;
                    case 'b':
                    case 'B':
                        base = 2;
                        ++it;
                        break;
                    case LNumNone:
                        return {};
                }
            }
        }

        uint64_t value = 0;

        while(it != itEnd) {
            int digit = -1;
            char c = *it;

            if(c >= '0' && c <= '9') digit = c - '0';
            else if(c >= 'a' && c <= 'f') digit = c - 'a' + 10;
            else if(c >= 'A' && c <= 'F') digit = c - 'A' + 10;

            if(digit != -1 && digit < base) {
                value = (value * base) + digit;
                ++it;
            }
            else {
                break;
            }
        }

        bool isUnsigned = false;

        while(it != itEnd) {
            char c = *it;
            if(c == 'u' || c == 'U') {
                isUnsigned = true;
            }
            else break;
            ++it;
        };

        return ExprInteger(value, isUnsigned);
    }

    Lexer& lexer;
    Lexer::LexerTokenStack tokStack;
    Token cur;
    bool atend{};
    bool err{};

    void consume() {
        if(atend) {
            cur = {TokenKind::TOKEN_END, 0, nullptr, nullptr};
            return;
        }
        if(tokStack.size()) {
            cur = *tokStack.back();
            tokStack.pop_back();
            return;
        }
        if(lexer.nextNoPreProcessingForce(cur)) {
            atend = true;
            consume();
            return;
        }
        if(lexer.preprocess(cur, tokStack)) {
            consume();
        }
    }

    void consumeNoPP() {
        if(atend) {
            cur = {TokenKind::TOKEN_END, 0, nullptr, nullptr};
            return;
        }
        if(tokStack.size()) {
            cur = *tokStack.back();
            tokStack.pop_back();
            return;
        }
        if(lexer.nextNoPreProcessingForce(cur)) {
            atend = true;
            consume();
            return;
        }
    }

    void terminate() {
        err = true;
        while(cur.getKind() != TokenKind::TOKEN_END) consume();
    }

public:
    CPPParser(Lexer& lex) noexcept : lexer(lex) {
        consume();
    }

    ExprInteger primary() {
        if(cur.getInfo()) {
            if(cur.getKind() == TokenKind::KEYWORD_defined) {
                ExprInteger result{false};
                consumeNoPP();

                bool parens = cur.getKind() == TokenKind::SYMBOL_LPAREN;
                if(parens) {
                    consumeNoPP();
                }

                if(IdentInfo* info = cur.getInfo()) {
                    if(lexer.macros_.find(info->getStart(), info->getLength()))
                        result = true;
                }
                else {
                    terminate();
                    return {};
                }
                parens ? consumeNoPP() : consume();

                if(parens) {
                    if(cur.getKind() != TokenKind::SYMBOL_RPAREN) {
                        terminate();
                        return {};
                    }
                    consume();
                }

                return result;
            }
            else consume();

            return {false};
        }

        if(cur.getKind() == TokenKind::LITERAL_INTEGER) {
            ExprInteger result = fromToken(cur.getStart(), cur.getLength());
            consume();
            return result;
        }

        if(cur.getKind() == TokenKind::SYMBOL_LPAREN) {
            consume();
            ExprInteger result = lor();
            if(cur.getKind() != TokenKind::SYMBOL_RPAREN) {
                terminate();
                return {};
            }
            consume();
            return result;
        }

        terminate();
        return {};
    }

    ExprInteger unary() {
        TokenKind kind = cur.getKind();

        if(kind == TokenKind::SYMBOL_EXCLAMATION) {
            consume();
            return !unary();
        }
        else if(kind == TokenKind::SYMBOL_TILDE) {
            consume();
            return ~unary();
        }
        else if(kind == TokenKind::SYMBOL_MINUS) {
            consume();
            return -unary();
        }
        else if(kind == TokenKind::SYMBOL_PLUS) {
            consume();
            return +unary();
        }

        return primary();
    }

    ExprInteger mul() {
        ExprInteger lhs = unary();
        while(true) {
            TokenKind kind = cur.getKind();
            if(kind == TokenKind::SYMBOL_STAR) {
                consume();
                lhs *= unary();
            }
            else if(kind == TokenKind::SYMBOL_SLASH) {
                consume();
                ExprInteger v = unary();
                if(v.getValue() != 0) {
                    lhs /= v;
                }
                else lhs = {}; // ERROR.
            }
            else if(kind == TokenKind::SYMBOL_PERCENT) {
                consume();
                ExprInteger v = unary();
                if(v.getValue() != 0) {
                    lhs %= v;
                }
                else lhs = {}; // ERROR.
            }
            else break;
        }

        return lhs;
    }

    ExprInteger add() {
        ExprInteger lhs = mul();
        while(true) {
            TokenKind kind = cur.getKind();
            if(kind == TokenKind::SYMBOL_PLUS) {
                consume();
                lhs += mul();
            }
            else if(kind == TokenKind::SYMBOL_MINUS) {
                consume();
                lhs -= mul();
            }
            else break;
        }

        return lhs;
    }

    ExprInteger shift() {
        ExprInteger lhs = add();
        while(true) {
            TokenKind kind = cur.getKind();
            if(kind == TokenKind::SYMBOL_SHIFTRIGHT) {
                consume();
                lhs >>= add();
            }
            else if(kind == TokenKind::SYMBOL_SHIFTLEFT) {
                consume();
                lhs <<= add();
            }
            else break;
        }

        return lhs;
    }

    ExprInteger rel() {
        ExprInteger lhs = shift();
        while(true) {
            TokenKind kind = cur.getKind();
            if(kind == TokenKind::SYMBOL_GREATER) {
                consume();
                lhs = lhs > shift();
            }
            else if(kind == TokenKind::SYMBOL_LESS) {
                consume();
                lhs = lhs < shift();
            }
            else if(kind == TokenKind::SYMBOL_GREATEREQUAL) {
                consume();
                lhs = lhs >= shift();
            }
            else if(kind == TokenKind::SYMBOL_LESSEQUAL) {
                consume();
                lhs = lhs <= shift();
            }
            else break;
        }

        return lhs;
    }

    ExprInteger eq() {
        ExprInteger lhs = rel();
        while(true) {
            TokenKind kind = cur.getKind();
            if(kind == TokenKind::SYMBOL_EQUALEQUAL) {
                consume();
                lhs = lhs == rel();
            }
            else if(kind == TokenKind::SYMBOL_NOTEQUAL) {
                consume();
                lhs = lhs != rel();
            }
            else break;
        }

        return lhs;
    }

    ExprInteger band() {
        ExprInteger lhs = eq();
        while(cur.getKind() == TokenKind::SYMBOL_AMPERSAND) {
            consume();
            lhs &= eq();
        }
        return lhs;
    }

    ExprInteger bxor() {
        ExprInteger lhs = band();
        while(cur.getKind() == TokenKind::SYMBOL_CARET) {
            consume();
            lhs ^= band();
        }
        return lhs;
    }

    ExprInteger bor() {
        ExprInteger lhs = bxor();
        while(cur.getKind() == TokenKind::SYMBOL_PIPE) {
            consume();
            lhs |= bxor();
        }
        return lhs;
    }

    ExprInteger land() {
        ExprInteger lhs = bor();
        while(cur.getKind() == TokenKind::SYMBOL_AND) {
            consume();

            ExprInteger rhs = bor();

            if(lhs.getValue() != 0) {
                lhs = lhs && rhs;
            }
        }
        return lhs;
    }

    ExprInteger lor() {
        ExprInteger lhs = land();
        while(cur.getKind() == TokenKind::SYMBOL_OR) {
            consume();

            ExprInteger rhs = land();

            if(lhs.getValue() == 0) {
                lhs = lhs || rhs;
            }
        }
        return lhs;
    }

    ExprInteger parse() {
        return lor();
    }

    bool hadErr() const noexcept {
        return err;
    }
};

int Lexer::parseIfExpr() {
    CPPParser parser(*this);
    ExprInteger result = parser.parse();

    return parser.hadErr() ? -1 : result.getValue() != false;
}

bool Lexer::nextNoPreProcessing(Token& to) {
    if(!tokenStack_.empty()) {
        to = *tokenStack_.back();
        tokenStack_.pop_back();
        return false;
    }
    if(skipWhitespacePreProcess()) return true;

    uint8_t c = lookupTable.lookup(*ptr_);

    to.setStart(ptr_);
    to.setIdentInfo(nullptr);

    if(c & LErr) {
        to.setKind(TokenKind::TOKEN_UNKNOWN_BYTES);
        do {
            skip();
            c = lookupTable.lookup(*ptr_);
        } while(c & LErr);
        to.setLength(to.getStart() - ptr_);
        // ERROR.
        return true;
    }

    switch((LexerLookup)c) {
        case LTerm:
            if(caseNone(to)) return nextNoPreProcessing(to);
            break;
        case LAlpha:
            caseAlpha(to);
            break;
        case LNum:
            caseNum(to);
            break;
        case LSymbol:
            if(caseSymbol(to, false)) return nextNoPreProcessing(to);
            break;
        case LWhitespace:
        case LErr:
        case LNL:
            inr_notpossible("This is checked beforehand.");
        case LNone:
            inr_notpossible("None are LNone.");
    }

    if(!tokensAllowed() && to.getKind() != TokenKind::TOKEN_END)
        return nextNoPreProcessing(to);

    return false;
}

Token Lexer::nextNoPreProcessing() {
    if(!tokenStack_.empty()) {
        Token tok = *tokenStack_.back();
        tokenStack_.pop_back();
        return tok;
    }
    skipWhitespace();

    uint8_t c = lookupTable.lookup(*ptr_);

    Token tok;
    tok.setStart(ptr_);

    if(c & LErr) {
        tok.setKind(TokenKind::TOKEN_UNKNOWN_BYTES);
        do {
            skip();
            c = lookupTable.lookup(*ptr_);
        } while(c & LErr);
        tok.setLength(tok.getStart() - ptr_);
        // ERROR.
        return tok;
    }

    switch((LexerLookup)c) {
        case LTerm:
            if(caseNone(tok)) return next();
            break;
        case LAlpha:
            caseAlpha(tok);
            break;
        case LNum:
            caseNum(tok);
            break;
        case LSymbol:
            if(caseSymbol(tok, false)) return next();
            break;
        case LWhitespace:
        case LErr:
        case LNL:
            inr_notpossible("This is checked beforehand.");
        case LNone:
            inr_notpossible("None are LNone.");
    }

    if(!tokensAllowed() && tok.getKind() != TokenKind::TOKEN_END) return next();

    return tok;
}

bool Lexer::nextNoPreProcessingForce(Token& to) {
    if(!tokenStack_.empty()) {
        to = *tokenStack_.back();
        tokenStack_.pop_back();
        return false;
    }
    if(skipWhitespacePreProcess()) return true;

    uint8_t c = lookupTable.lookup(*ptr_);

    to.setStart(ptr_);
    to.setIdentInfo(nullptr);

    if(c & LErr) {
        to.setKind(TokenKind::TOKEN_UNKNOWN_BYTES);
        do {
            skip();
            c = lookupTable.lookup(*ptr_);
        } while(c & LErr);
        to.setLength(to.getStart() - ptr_);
        // ERROR.
        return true;
    }

    switch((LexerLookup)c) {
        case LTerm:
            if(caseNone(to)) return nextNoPreProcessingForce(to);
            break;
        case LAlpha:
            caseAlpha(to);
            break;
        case LNum:
            caseNum(to);
            break;
        case LSymbol:
            if(caseSymbol(to, false)) return nextNoPreProcessingForce(to);
            break;
        case LWhitespace:
        case LErr:
        case LNL:
            inr_notpossible("This is checked beforehand.");
        case LNone:
            inr_notpossible("None are LNone.");
    }

    return false;
}

Token Lexer::next() {
    if(!tokenStack_.empty()) {
        Token tok = *tokenStack_.back();
        tokenStack_.pop_back();
        return tok;
    }
    skipWhitespace();

    uint8_t c = lookupTable.lookup(*ptr_);

    Token tok;
    tok.setStart(ptr_);

    if(c & LErr) {
        tok.setKind(TokenKind::TOKEN_UNKNOWN_BYTES);
        do {
            skip();
            c = lookupTable.lookup(*ptr_);
        } while(c & LErr);
        tok.setLength(tok.getStart() - ptr_);
        // ERROR.
        return tok;
    }

    switch((LexerLookup)c) {
        case LTerm:
            if(caseNone(tok)) return next();
            break;
        case LAlpha:
            caseAlpha(tok);
            break;
        case LNum:
            caseNum(tok);
            break;
        case LSymbol:
            if(caseSymbol(tok)) return next();
            break;
        case LWhitespace:
        case LErr:
        case LNL:
            inr_notpossible("This is checked beforehand.");
        case LNone:
            inr_notpossible("None are LNone.");
    }

    if(!tokensAllowed() && tok.getKind() != TokenKind::TOKEN_END) return next();

    return preprocess(tok, tokenStack_) ? next() : tok;
}

Token Lexer::nextNoPreProcessingDirective() {
    if(!tokenStack_.empty()) {
        Token tok = *tokenStack_.back();
        tokenStack_.pop_back();
        return tok;
    }
    skipWhitespace();

    uint8_t c = lookupTable.lookup(*ptr_);

    Token tok;
    tok.setStart(ptr_);

    if(c & LErr) {
        tok.setKind(TokenKind::TOKEN_UNKNOWN_BYTES);
        do {
            skip();
            c = lookupTable.lookup(*ptr_);
        } while(c & LErr);
        tok.setLength(tok.getStart() - ptr_);
        // ERROR.
        return tok;
    }

    switch((LexerLookup)c) {
        case LTerm:
            if(caseNone(tok)) return nextNoPreProcessingDirective();
            break;
        case LAlpha:
            caseAlpha(tok);
            break;
        case LNum:
            caseNum(tok);
            break;
        case LSymbol:
            if(caseSymbol(tok, false)) return nextNoPreProcessingDirective();
            break;
        case LWhitespace:
        case LErr:
        case LNL:
            inr_notpossible("This is checked beforehand.");
        case LNone:
            inr_notpossible("None are LNone.");
    }

    if(!tokensAllowed() && tok.getKind() != TokenKind::TOKEN_END)
        return nextNoPreProcessingDirective();

    return preprocess(tok, tokenStack_) ? nextNoPreProcessingDirective() : tok;
}

} // namespace inrcc