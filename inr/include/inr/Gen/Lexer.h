#ifndef INERTIA_GEN_LEXER_H
#define INERTIA_GEN_LEXER_H

/// @file Gen/Lexer.h
/// @brief Contains the lexer for inr-gen.

#include <inr/ADT/StrView.h>

#include <charconv>
#include <cstdint>
#include <list>

namespace inr::gen {

/// @brief Represents a span in memory with a certain type.
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
    /// @brief Basic token constructor.
    /// @param start Pointer to the start.
    /// @param end Pointer to the end.
    /// @param id Token type.
    /// @param extra Extra flags, can be read above.
    token(const char* start, const char* end, ID id, uint8_t extra = 0) noexcept
        :
        start_(start), end_(end), id_(id), extra_(extra) {}

    /// @brief Set the end pointer of the token.
    /// @param end New end pointer.
    void setEnd(const char* end) {
        end_ = end;
    };

    /// @brief Get the pointer to the start.
    /// @return Pointer to the start.
    const char* getStart() const noexcept {
        return start_;
    }

    /// @brief Is the token a double type?
    /// @return True if yes, false if no.
    bool isDouble() const noexcept {
        return extra_ != 0;
    }

    /// @brief Get base of the integer inside.
    /// @return Base.
    unsigned getBase() const noexcept {
        return extra_;
    }

    /// @brief Get the pointer to the end.
    /// @return Pointer to the end.
    const char* getEnd() const noexcept {
        return end_;
    }

    /// @brief Get the token type.
    /// @return Type of the token.
    ID getID() const noexcept {
        return id_;
    }

    /// @brief Get the token as a string view.
    /// @return String view.
    sview getAsString() const noexcept {
        return sview(start_, end_ - start_);
    }

    /// @brief Gets the token as an integer.
    /// @return Integer from the token.
    /// @note The base is handled internally.
    uint64_t getAsInteger() const noexcept {
        uint64_t v;

        unsigned offset = 0;

        uint8_t base = getBase();
        if(base == 16 || base == 2) offset += 2;

        std::from_chars(start_ + offset, end_, v, extra_);
        return v;
    }

    /// @brief Gets the token as a float.
    /// @return Float from the token.
    float getAsFloat() const noexcept {
        float d;
        std::from_chars(start_, end_, d);
        return d;
    }

    /// @brief Gets the token as a double.
    /// @return Double from the token.
    double getAsDouble() const noexcept {
        double d;
        std::from_chars(start_, end_, d);
        return d;
    }

    /// @brief Gets the token as a char.
    /// @return Char from the token.
    char getAsChar() const noexcept {
        return *start_;
    }
};

/// @brief Class that lexes a inr-gen file.
/// @note Can only be used once.
class lexer {
    /// @brief Container for the tokens.
    std::list<token> tokens_;
    /// @brief Name of the file that is being lexed.
    sview fileName_;
    /// @brief Iterator that is used internally.
    const char* start_;
    /// @brief Pointer to the end.
    const char* end_;
    /// @brief Current column in the file.
    unsigned long column_;
    /// @brief Current line in the file.
    unsigned long line_;

    /// @brief Advance forward and process new lines and columns.
    void advance() noexcept;
    /// @brief Skip whitespace until the next non-whitespace char.
    void skipWhiteSpace() noexcept;
    /// @brief Skip until '\n' is encountered.
    void skipLineComments() noexcept;
    /// @brief Skip alphanumerical characters.
    void skipAlnum() noexcept;
    /// @brief Advance forward until a character is encountered.
    /// @param c Character to look for.
    /// @param escape Should it ignore the character if last char was backslash.
    void advanceUntil(char c, bool escape) noexcept;
    /// @brief Lex the current token as an alpha.
    void lexAlpha();
    /// @brief Lex the current token as a symbol.
    void lexSymbol();
    /// @brief Lex the current token as a number.
    void lexNumber();
    /// @brief Lex the current token as a string.
    void lexString();
    /// @brief Outputs an error message of encountering an unknown char.
    /// @param adv Advance forward after outputting.
    void uknownChar(bool adv = false);

    /// @brief Classify the current token to a symbol.
    /// @return The correct token type if found, `token::ID::End` if not found.
    token::ID classifySymbol() const noexcept;
    /// @brief Classify the string to a keyword.
    /// @param sv The string to classify.
    /// @return The correct token type if found, `token::ID::End` if not found.
    token::ID classifyAlpha(sview sv);

    /// @brief Returns whether the current char is a symbol.
    /// @return True if is a symbol, false if not.
    bool isSymbol() const noexcept;
    /// @brief Peeks a char forward.
    /// @return Next char if possible, 0 if not.
    char peek() const noexcept;

    /// @brief Gets the current char.
    /// @return This char if possible, 0 if not.
    char getChar() const noexcept {
        return start_ != end_ ? *start_ : '\0';
    }

    /// @brief Compares this char to the expectation.
    /// @param c Expectation.
    /// @return True if the expectation is met.
    bool expect(char c) const noexcept {
        return getChar() == c;
    }

    /// @brief Add new token to the back of the list.
    /// @see `token` constructor for more info.
    void addToken(const char* from, const char* to, token::ID id,
                  uint8_t extra = 0) {
        tokens_.emplace_back(from, to, id, extra);
    }

    /// @brief Used internally.
    /// @see `lex()` to get more info.
    std::list<token> internalLex();

public:
    /// @brief Creates a new lexer.
    /// @param fileName Name of the file that is being lexed.
    /// @param start Pointer to the start.
    /// @param end Pointer to the end.
    lexer(sview fileName, const char* start, const char* end) noexcept :
        fileName_(fileName), start_(start), end_(end), column_(0), line_(1) {}

    /// @brief Lexes the file in the lexer.
    /// @note Can only be used once.
    /// @return A list of the tokens.
    std::list<token> lex() {
        return internalLex();
    }
};

class raw_stream& operator<<(raw_stream&, const token&);

} // namespace inr::gen

#endif // INERTIA_GEN_LEXER_H
