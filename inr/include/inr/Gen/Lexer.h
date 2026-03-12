#ifndef INERTIA_GEN_LEXER_H
#define INERTIA_GEN_LEXER_H

/// @file Gen/Lexer.h
/// @brief Contains the lexer for inr-gen.

#include <inr/ADT/StrView.h>

#include <charconv>
#include <cstdint>
#include <vector>

namespace inr::gen {

class token {
public:
    /// @brief The type of the token.
    enum class ID {
        IntegerLiteral,
        FloatLiteral,
        CharLiteral,
        StringLiteral,
        Identifier,
        LeftArrow,
        RightArrow,
        Comma,
        Semicolon,
        LeftParen,
        RightParen,
        LeftBrace,
        RightBrace,
        Little,
        Big,
        New,
        Instruction,
        InstructionType,
        Operand,
        Integer,
        Colon,
        Endian,
        Pointer,
        End
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
    uint8_t extra_;

public:
    const char* getStart() const noexcept {
        return start_;
    }

    bool isDouble() const noexcept {
        return extra_ != 0;
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
        std::from_chars(start_, end_, v, extra_);
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

class lexer {
public:
    static std::vector<token> lex(const char* start, const char* end);
};

class raw_stream& operator<<(raw_stream&, const token&);

} // namespace inr::gen

#endif // INERTIA_GEN_LEXER_H
