#ifndef INERTIA_GEN_LEXER_H
#define INERTIA_GEN_LEXER_H

/// @file Gen/Lexer.h
/// @brief Contains the lexer for inr-gen.

#include <inr/ADT/StrView.h>

#include <charconv>
#include <cstdint>
#include <list>

namespace inr::gen {

class token {
public:
    /// @brief The type of the token.
    enum class ID {
        IntegerLiteral,  ///< Integer literal (e.g. 42).
        FloatLiteral,    ///< Float literal, unsupported.
        CharLiteral,     ///< Char literal, unsupported.
        StringLiteral,   ///< String literal (e.g. "world").
        Identifier,      ///< Alphanumerical word.
        LeftArrow,       ///< '<'
        RightArrow,      ///< '>'
        Comma,           ///< ','
        Semicolon,       ///< ';'
        LeftParen,       ///< '('
        RightParen,      ///< ')'
        LeftBrace,       ///< '{'
        RightBrace,      ///< '}'
        LeftSquare,      ///< '['
        RightSquare,     ///< ']'
        Little,          ///< little keyword.
        Big,             ///< big keyword.
        New,             ///< new keyword.
        Instruction,     ///< Instruction keyword.
        InstructionType, ///< InstructionType keyword.
        Operand,         ///< Operand keyword.
        Integer,         ///< integer keyword.
        Define,          ///< define keyword.
        Target,          ///< target keyword.
        Colon,           ///< ':'
        Slash,           ///< '/'
        End              ///< Error token.
    };

private:
    /// @brief Start of the token.
    const char* start_;
    /// @brief End of the token.
    const char* end_;
    /// @brief Type of the token.
    ID id_;
    /// @brief Extra data, explained below.
    ///
    /// Extra data can be one of the following:
    /// - Integer Base.
    /// - Is double or float.
    uint8_t extra_;

public:
    token(const char* start, const char* end, ID id, uint8_t extra = 0) noexcept
        :
        start_(start), end_(end), id_(id), extra_(extra) {}

    void setEnd(const char* end) {
        end_ = end;
    };

    const char* getStart() const noexcept {
        return start_;
    }

    bool isDouble() const noexcept {
        return extra_ != 0;
    }

    int getBase() const noexcept {
        return extra_;
    }

    const char* getEnd() const noexcept {
        return end_;
    }

    ID getID() const noexcept {
        return id_;
    }

    sview getAsString() const noexcept {
        return sview(start_, end_ - start_);
    }

    uint64_t getAsInteger() const noexcept {
        uint64_t v;

        unsigned offset = 0;

        uint8_t base = getBase();
        if(base == 16 || base == 2) offset += 2;

        std::from_chars(start_ + offset, end_, v, extra_);
        return v;
    }

    float getAsFloat() const noexcept {
        float d;
        std::from_chars(start_, end_, d);
        return d;
    }

    double getAsDouble() const noexcept {
        double d;
        std::from_chars(start_, end_, d);
        return d;
    }

    char getAsChar() const noexcept {
        return *start_;
    }
};

/// @brief Class that lexes a inrgen file.
class lexer {
    std::list<token> tokens_;
    sview fileName_;
    const char* start_;
    const char* end_;
    unsigned long column_;
    unsigned long line_;

    void advance() noexcept;
    void skipWhiteSpace() noexcept;
    void skipLineComments() noexcept;
    void skipAlnum() noexcept;
    void advanceUntil(char c, bool escape) noexcept;
    void lexAlpha();
    void lexSymbol();
    void lexNumber();
    void lexString();
    void uknownChar(bool adv = false);

    token::ID classifySymbol() const noexcept;
    token::ID classifyAlpha(sview sv);

    bool isSymbol() const noexcept;
    char peek() const noexcept;

    char getChar() const noexcept {
        return start_ != end_ ? *start_ : '\0';
    }
    bool expect(char c) const noexcept {
        return getChar() == c;
    }

    void addToken(const char* from, const char* to, token::ID id,
                  uint8_t extra = 0) {
        tokens_.emplace_back(from, to, id, extra);
    }

    std::list<token> internalLex();

public:
    lexer(sview fileName, const char* start, const char* end) noexcept :
        fileName_(fileName), start_(start), end_(end), column_(0), line_(1) {}

    /// @brief Lexes the file in the lexer.
    /// @note Can only be used once.
    std::list<token> lex() {
        return internalLex();
    }
};

class raw_stream& operator<<(raw_stream&, const token&);

} // namespace inr::gen

#endif // INERTIA_GEN_LEXER_H
