#include <inr/ADT/StrView.h>
#include <inr/Gen/Lexer.h>
#include <inr/Support/Logger.h>
#include <inr/Support/Stream.h>

#include <cctype>
#include <utility>

namespace inr::gen {

raw_stream& operator<<(raw_stream& os, const token& token) {
    switch(token.getID()) {
        case token::ID::IntegerLiteral:
            return os << token.getAsInteger();
        case token::ID::FloatLiteral:
            if(token.isDouble()) return os << token.getAsDouble();
            else return os << token.getAsFloat();
        case token::ID::CharLiteral:
            [[fallthrough]];
        case token::ID::LeftArrow:
            [[fallthrough]];
        case token::ID::RightArrow:
            [[fallthrough]];
        case token::ID::Comma:
            [[fallthrough]];
        case token::ID::Semicolon:
            [[fallthrough]];
        case token::ID::LeftParen:
            [[fallthrough]];
        case token::ID::RightParen:
            [[fallthrough]];
        case token::ID::LeftBrace:
            [[fallthrough]];
        case token::ID::RightBrace:
            [[fallthrough]];
        case token::ID::Colon:
            [[fallthrough]];
        case token::ID::LeftSquare:
            [[fallthrough]];
        case token::ID::RightSquare:
            return os << token.getAsChar();
        case token::ID::StringLiteral:
            [[fallthrough]];
        case token::ID::Little:
            [[fallthrough]];
        case token::ID::Big:
            [[fallthrough]];
        case token::ID::New:
            [[fallthrough]];
        case token::ID::Instruction:
            [[fallthrough]];
        case token::ID::InstructionType:
            [[fallthrough]];
        case token::ID::Operand:
            [[fallthrough]];
        case token::ID::Integer:
            [[fallthrough]];
        case token::ID::Identifier:
            [[fallthrough]];
        case token::ID::Target:
            [[fallthrough]];
        case token::ID::Define:
            return os << token.getAsString();
        case token::ID::End:
        case token::ID::Slash: // Used for comments
            return os;
    }
}

void lexer::advance() noexcept {
    if(start_ == end_) return;
    char c = getChar();

    if(c == '\n') {
        line_++;
        column_ = 0;
    }
    else {
        column_++;
    }

    start_++;
}

char lexer::peek() const noexcept {
    if(start_ + 1 != end_) {
        return start_[1];
    }
    return '\0';
}

void lexer::skipLineComments() noexcept {
    advanceUntil('\n', false);
}

void lexer::uknownChar(bool adv) {
    static char formattedMsg[] = "unknown character ' ' encountered";
    constexpr static sview formattedSview(formattedMsg, sizeof(formattedMsg));

    formattedMsg[19] = getChar();

    log::sendpos(inr::errs(), log::Level::ERROR, fileName_, line_, column_,
                 formattedSview);
    if(adv) advance();
}

void lexer::skipWhiteSpace() noexcept {
    while(std::isspace((unsigned char)getChar()) && start_ != end_) advance();
}

void lexer::skipAlnum() noexcept {
    while(std::isalnum((unsigned char)getChar()) && start_ != end_) advance();
}

void lexer::advanceUntil(char c, bool escape) noexcept {
    bool bs = false; // Backslash
    while(start_ != end_) {
        if(expect(c)) {
            if(!bs) {
                return;
            }
        }

        if(bs) bs = false;
        if(expect('\\') && escape) bs = true;

        advance();
    }
}

constexpr std::pair<sview, token::ID> keywords[] = {
    {"little", token::ID::Little},
    {"big", token::ID::Big},
    {"new", token::ID::New},
    {"Instruction", token::ID::Instruction},
    {"InstructionType", token::ID::InstructionType},
    {"Operand", token::ID::Operand},
    {"integer", token::ID::Integer},
    {"define", token::ID::Define},
    {"target", token::ID::Target},
};

token::ID lexer::classifyAlpha(sview sv) {
    for(const auto& p : keywords) {
        if(p.first == sv) {
            return p.second;
        }
    }
    return token::ID::Identifier;
}

void lexer::lexAlpha() {
    const char* start = start_;

    skipAlnum();

    addToken(start, start_, classifyAlpha(sview(start, start_ - start)));
}

void lexer::lexString() {
    const char* start = start_;

    advanceUntil('"', true);
    if(start_ == end_) {
        log::sendpos(errs(), log::Level::ERROR, fileName_, line_, column_,
                     "string abruptly ended");
        return;
    }

    addToken(start, start_, token::ID::StringLiteral);
    advance();
}

void lexer::lexSymbol() {
    const char* start = start_;

    token::ID id = classifySymbol();
    if(id == token::ID::Slash) {
        if(peek() == '/') {
            skipLineComments();
        }
        else {
            log::sendpos(errs(), log::Level::ERROR, fileName_, line_, column_,
                         "slash is only used comments (//)");
            advance();
        }
        return;
    }

    advance();
    if(id == token::ID::StringLiteral) {
        lexString();
        return;
    }

    addToken(start, start_, id);
}

void lexer::lexNumber() {
    const char* start = start_;

    skipAlnum();

    token::ID id = token::ID::IntegerLiteral;

    if(expect('.')) {
        log::sendpos(errs(), log::Level::ERROR, fileName_, line_, column_,
                     "floating point is not supported");
        advance();
        return;
    }

    uint8_t base = 10;

    sview sw(start, start_ - start);

    if(sw.size() > 1) {
        size_t checkIdx = 0;
        if(sw[0] == '0') {
            switch(sw[1]) {
                case 'x':
                case 'X':
                    base = 16;
                    checkIdx = 2;
                    break;
                case 'b':
                case 'B':
                    base = 2;
                    checkIdx = 2;
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
                default:
                    log::sendpos(outs(), log::Level::ERROR, fileName_, line_,
                                 column_, "unknown prefix in the literal");
                    return;
            }
        }

        for(auto it = sw.begin() + checkIdx; it != sw.end(); ++it) {
            char c = *it;

            if(base == 0b10) {
                if(c != '0' && c != '1') {
                    log::sendpos(
                        errs(), log::Level::ERROR, fileName_, line_, column_,
                        "binary literals cannot have non binary digits");
                    return;
                }
            }
            else if(base == 8) {
                if(c > '7' || c < '0') {
                    log::sendpos(errs(), log::Level::ERROR, fileName_, line_,
                                 column_,
                                 "octal literals cannot have non octal digits");
                    return;
                }
            }
            else if(base == 10) {
                if(!std::isdigit((unsigned char)c)) {
                    log::sendpos(
                        errs(), log::Level::ERROR, fileName_, line_, column_,
                        "decimal literals cannot have non decimal digits");
                    return;
                }
            }
            else if(base == 16) {
                if(!std::isxdigit((unsigned char)c)) {
                    log::sendpos(errs(), log::Level::ERROR, fileName_, line_,
                                 column_,
                                 "hexadecimal literals cannot have non "
                                 "hexadecimal digits");
                    return;
                }
            }
        }
    }

    addToken(sw.begin(), sw.end(), id, base);
}

token::ID lexer::classifySymbol() const noexcept {
    switch(getChar()) {
        case '(':
            return token::ID::LeftParen;
        case ')':
            return token::ID::RightParen;
        case ',':
            return token::ID::Comma;
        case ':':
            return token::ID::Colon;
        case '<':
            return token::ID::LeftArrow;
        case '>':
            return token::ID::RightArrow;
        case '{':
            return token::ID::LeftBrace;
        case '}':
            return token::ID::RightBrace;
        case '[':
            return token::ID::LeftSquare;
        case ']':
            return token::ID::RightSquare;
        case ';':
            return token::ID::Semicolon;
        case '"':
            return token::ID::StringLiteral;
        case '/':
            return token::ID::Slash;
        default:
            return token::ID::End;
    }
}

bool lexer::isSymbol() const noexcept {
    return classifySymbol() != token::ID::End;
}

std::list<token> lexer::internalLex() {
    while(start_ != end_) {
        skipWhiteSpace();

        if(std::isalpha((unsigned char)getChar())) {
            lexAlpha();
        }
        else if(isSymbol()) {
            lexSymbol();
        }
        else if(std::isdigit((unsigned char)getChar())) {
            lexNumber();
        }
        else {
            if(start_ != end_) uknownChar(true);
        }
    }

    return std::move(tokens_);
}

} // namespace inr::gen