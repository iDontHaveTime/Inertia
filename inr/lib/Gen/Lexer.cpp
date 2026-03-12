#include <inr/Gen/Lexer.h>
#include <inr/Support/Stream.h>

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
        case token::ID::Endian:
            [[fallthrough]];
        case token::ID::Pointer:
            return os << token.getAsString();
        case token::ID::End:
            return os;
    }
}

enum CharType : uint8_t { Alpha, Symbol, Digit, Whitespace };

struct CharTypeArray {
    CharType arr[0x100];
    constexpr CharType classifyChar(uint8_t c) const noexcept {
        if(c >= 'a' && c <= 'z') {
            return Alpha;
        }
        else if(c >= '0' && c <= '9') {
            return Digit;
        }
    
        return Whitespace;
    }

    constexpr CharTypeArray() noexcept : arr() {
        for(size_t i = 0; i < 0x100; i++) {
            arr[i] = classifyChar(i);

            sview symbols = "!@#$%^&*()_+-=[]{}\\|/?.>,<'\";:~`";
            for(char c : symbols) {
                arr[(uint8_t)c] = Symbol;
            }
        }
    }
};

constexpr CharTypeArray charClass;

struct LexerContext {
    std::vector<token> vec;
    char c;
    CharType cT;
    const char *start, *end;
    size_t line, column;
    token* currentToken;

    LexerContext(const char* start_, const char* end_) noexcept :
        vec(), c(), cT(), start(start_), end(end_), line(1), column(0) {}
};

static inline void lexWhitespace(LexerContext& ctx) noexcept {
    if(ctx.c == '\n') {
        ctx.line++;
        ctx.column = 0;
    }
}

static inline void lexJump(LexerContext& ctx) noexcept {
    switch(ctx.cT) {
        case Alpha:
            ctx.column++;
            break;
        case Symbol:
            ctx.column++;
            break;
        case Digit:
            ctx.column++;
            break;
        case Whitespace:
            ctx.column++;
            lexWhitespace(ctx);
            break;
    }
}

std::vector<token> lexer::lex(const char* start, const char* end) {
    LexerContext ctx(start, end);

    while(ctx.start != ctx.end) {
        ctx.c = *start;
        ctx.cT = charClass.arr[(uint8_t)ctx.c];
        lexJump(ctx);
        ctx.start++;
    }

    return std::move(ctx.vec);
}

} // namespace inr::gen