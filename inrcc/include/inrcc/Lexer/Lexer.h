// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_LEXER_LEXER_H
#define INRCC_LEXER_LEXER_H

/// @file Lexer/Lexer.h
/// @brief A lexer specifically made for C family languages.

#include <inr/ADT/ArrView.h>
#include <inr/ADT/IVector.h>
#include <inr/ADT/StrView.h>
#include <inr/Target/Triple.h>
#include <inrcc/ADT/StringMap.h>
#include <inrcc/Driver/DriverFileManager.h>
#include <inrcc/Options/Data.h>
#include <inrcc/Options/LangOptions.h>
#include <inrcc/Support/Arena.h>

namespace inrcc {

/// @brief Represents a token kind.
enum class TokenKind : unsigned {
    TOKEN_UNKNOWN = 0,
    TOKEN_UNKNOWN_BYTES = 0,
    TOKEN_IDENT,
    SYMBOL_EXCLAMATION,
    SYMBOL_LPAREN,
    SYMBOL_RPAREN,
    SYMBOL_LSQUARE,
    SYMBOL_RSQUARE,
    SYMBOL_LBRACE,
    SYMBOL_RBRACE,
    SYMBOL_PERCENT,
    SYMBOL_AMPERSAND,
    SYMBOL_STAR,
    SYMBOL_PLUS,
    SYMBOL_COMMA,
    SYMBOL_MINUS,
    SYMBOL_DOT,
    SYMBOL_SLASH,
    SYMBOL_COLON,
    SYMBOL_SEMICOLON,
    SYMBOL_LESS,
    SYMBOL_EQUAL,
    SYMBOL_GREATER,
    SYMBOL_QUESTION,
    SYMBOL_CARET,
    SYMBOL_PIPE,
    SYMBOL_TILDE,
    SYMBOL_AND,
    SYMBOL_OR,
    SYMBOL_PLUSPLUS,
    SYMBOL_MINUSMINUS,
    SYMBOL_EQUALEQUAL,
    SYMBOL_NOTEQUAL,
    SYMBOL_GREATEREQUAL,
    SYMBOL_LESSEQUAL,
    SYMBOL_SHIFTLEFT,
    SYMBOL_SHIFTRIGHT,
    SYMBOL_ARROW,
    SYMBOL_PLUSEQUAL,
    SYMBOL_MINUSEQUAL,
    SYMBOL_STAREQUAL,
    SYMBOL_SLASHEQUAL,
    SYMBOL_PERCENTEQUAL,
    SYMBOL_AMPERSANDEQUAL,
    SYMBOL_PIPEEQUAL,
    SYMBOL_CARETEQUAL,
    SYMBOL_SHIFTLEFTEQUAL,
    SYMBOL_SHIFTRIGHTEQUAL,
    SYMBOL_ELLIPSIS,
    SYMBOL_SPACESHIP,
    PREPROCESS_BACKSLASH,
    PREPROCESS_HASH,
    PREPROCESS_HASHHASH,
    LITERAL_INTEGER,
    LITERAL_STRING,
#define INRCC_KEYWORD(ID, ...) KEYWORD_##ID,
#include <inrcc/Lexer/TokenKind.inc>
#undef INRCC_KEYWORD
    KEYWORD___VA_ARGS__,
    TOKEN_END
};

class SourceLoc {
    unsigned long index_;

public:
    SourceLoc() noexcept = default;
    SourceLoc(long index) noexcept : index_(index) {}

    unsigned long getIndex() const noexcept {
        return index_;
    }
};

class IdentInfo {
    const char* start_;
    TokenKind kind_;
    uint32_t len_;

public:
    constexpr IdentInfo(TokenKind kind, const char* start,
                        uint32_t len) noexcept :
        start_(start), kind_(kind), len_(len) {}

    constexpr IdentInfo(const IdentInfo&) noexcept = default;
    constexpr IdentInfo& operator=(const IdentInfo&) noexcept = default;

    constexpr IdentInfo(IdentInfo&&) noexcept = default;
    constexpr IdentInfo& operator=(IdentInfo&&) noexcept = default;

    constexpr void setKind(TokenKind kind) noexcept {
        kind_ = kind;
    }

    constexpr void setStart(const char* start) noexcept {
        start_ = start;
    }

    constexpr void setLength(uint32_t len) noexcept {
        len_ = len;
    }

    constexpr TokenKind getKind() const noexcept {
        return kind_;
    }

    constexpr const char* getStart() const noexcept {
        return start_;
    }

    constexpr uint32_t getLength() const noexcept {
        return len_;
    }

    constexpr inr::sview getView() const noexcept {
        return {start_, len_};
    }
};

class TokenCharIterator {
    const char* ptr_;

public:
    TokenCharIterator(const char* ptr) noexcept : ptr_(ptr) {}

    TokenCharIterator(const TokenCharIterator&) noexcept = default;
    TokenCharIterator(TokenCharIterator&&) noexcept = default;

    TokenCharIterator& operator=(const TokenCharIterator&) noexcept = default;
    TokenCharIterator& operator=(TokenCharIterator&&) noexcept = default;

    char operator*() const noexcept {
        return *ptr_;
    }

    bool operator!=(TokenCharIterator other) const noexcept {
        return ptr_ != other.ptr_;
    }

    bool operator==(TokenCharIterator other) const noexcept {
        return ptr_ == other.ptr_;
    }

    TokenCharIterator& operator++() noexcept;

    static inline bool TokenCompareFunction(TokenCharIterator it1,
                                            TokenCharIterator it1End,
                                            TokenCharIterator it2,
                                            TokenCharIterator it2End) {
        while(it1 != it1End && it2 != it2End) {
            if(*it1 != *it2) return false;
            ++it1;
            ++it2;
        }
        return it1 == it1End && it2 == it2End;
    }
};

class Token {
    TokenKind kind_;
    uint32_t len_;
    const char* start_;
    IdentInfo* info_ = nullptr;

public:
    constexpr Token(TokenKind tk, uint32_t ln, const char* st,
                    IdentInfo* in) noexcept :
        kind_(tk), len_(ln), start_(st), info_(in) {}
    constexpr Token() noexcept = default;

    constexpr Token(IdentInfo* info) noexcept :
        kind_(info->getKind()),
        len_(info->getLength()),
        start_(info->getStart()),
        info_(info) {}

    constexpr void setKind(TokenKind kind) noexcept {
        kind_ = kind;
    }

    constexpr void setStart(const char* start) noexcept {
        start_ = start;
    }

    constexpr void setLength(uint32_t len) noexcept {
        len_ = len;
    }

    constexpr void setIdentInfo(IdentInfo* info) noexcept {
        info_ = info;
    }

    constexpr void setView(inr::sview str) noexcept {
        start_ = str.data();
        len_ = str.size();
    }

    constexpr TokenKind getKind() const noexcept {
        return kind_;
    }

    constexpr const char* getStart() const noexcept {
        return start_;
    }

    constexpr uint32_t getLength() const noexcept {
        return len_;
    }

    constexpr IdentInfo* getInfo() const noexcept {
        return info_;
    }

    constexpr inr::sview getView() const noexcept {
        return {start_, len_};
    }

    friend inr::raw_stream& operator<<(inr::raw_stream& os, const Token& tok) {
        if(tok.getKind() == TokenKind::LITERAL_STRING) {
            return os << '"' << tok.getView() << '"';
        }
        return os << tok.getView();
    }
};

class MacroInfo {
public:
    using MacroReplacements = inr::ivec<Token, 4>;

private:
    bool functionLike_ = false;
    bool isVararg_ = false;
    IdentInfo* defaultVarargIdent_ = nullptr;

    MacroReplacements replacements_;
    inr::ivec<IdentInfo*, 4> args_;

public:
    MacroInfo() noexcept = default;
    MacroInfo(std::initializer_list<Token> tokens) noexcept :
        replacements_(tokens) {}

    bool isFunctionLike() const noexcept {
        return functionLike_;
    }

    void enableFunctionLike() noexcept {
        functionLike_ = true;
    }

    bool isVararg() const noexcept {
        return isVararg_;
    }

    void enableVararg() noexcept {
        isVararg_ = true;
    }

    IdentInfo* getVarargIdent() const noexcept {
        return defaultVarargIdent_;
    }

    void setVarargIdent(IdentInfo* info) noexcept {
        defaultVarargIdent_ = info;
    }

    Token& newToken() noexcept {
        return replacements_.emplace_back();
    }

    const auto& getReplacements() const noexcept {
        return replacements_;
    }

    auto& getReplacements() noexcept {
        return replacements_;
    }

    const auto& getArgs() const noexcept {
        return args_;
    }

    auto& getArgs() noexcept {
        return args_;
    }

    static IdentInfo* getDefaultMacroIdentVararg();
};

using IdentMap = StringMap<IdentInfo*, TokenCharIterator,
                           TokenCharIterator::TokenCompareFunction, true>;
using MacroMap = StringMap<MacroInfo*, TokenCharIterator,
                           TokenCharIterator::TokenCompareFunction, true>;

/// @brief C Preprocessor parser.
class CPPParser;

/// @brief Lexes C-style languages.
/// @note Buffer must be null terminated.
class Lexer {
public:
    using LexerTokenStack = inr::ivec<const Token*, 8>;
    using LexerConditionsStack = inr::ivec<bool, 8>;
    using LexerMacroHideSet = inr::ivec<const MacroInfo*, 8>;

private:
    struct LexerContext {
        const char* ptr_;
        DriverFMan::File file_;
    };

    using LexerIncludeStack = inr::ivec<LexerContext, 8>;

    Language lang_;
    DriverFMan::File original_;
    LexerIncludeStack includeStack_;
    LexerConditionsStack conditionsStack_;
    LexerTokenStack tokenStack_;
    LexerMacroHideSet hideSet_;
    const char* ptr_;
    DriverFMan& fman_;
    Arena& arena_;
    IdentMap& infoTable_;
    MacroMap macros_;
    MacroInfo fileMacro_{Token(TokenKind::LITERAL_STRING, 0, nullptr, nullptr)};

    void setKeywords();
    void setMacros();

    void setBaseFileMacro(MacroInfo* macro) {
        macros_.insert("__BASE_FILE__", (sizeof("__BASE_FILE__") - 1), macro);
    }

    void setFileMacro(DriverFMan::File f) {
        fileMacro_.getReplacements().front().setView(f.originalName);
    }

    bool caseNone(Token& tok);
    void caseAlpha(Token& tok);
    void caseNum(Token& tok);
    bool caseSymbol(Token& tok, bool preprocess = true);
    void caseString(Token& tok);

    void routeLexing(uint8_t);

    void handlePreprocessing();

    void handleInclude();

    void handleDefine();
    void handleUndef();

    void handleIfDef(bool invert);

    int parseIfExpr();

    /// @brief Returns true if encountered a new line.
    bool lexUntilCOrNL(char c, bool escape);

    bool preprocess(const Token&, LexerTokenStack&);

    void pushFilePtr(const char* ptr, DriverFMan::File file) noexcept {
        includeStack_.emplace_back(ptr_, file);
        ptr_ = ptr;
        setFileMacro(file);
    }

    void popFilePtr() noexcept {
        ptr_ = includeStack_.back().ptr_;
        includeStack_.pop_back();
        if(includeStack_.size()) {
            setFileMacro(includeStack_.back().file_);
        }
        else setFileMacro(original_);
    }

    bool tokensAllowed() const noexcept {
        return std::all_of(conditionsStack_.begin(), conditionsStack_.end(),
                           [](bool v) { return v; });
    }

    bool skipElse() const noexcept {
        if(conditionsStack_.size() > 1) {
            return !conditionsStack_[conditionsStack_.size() - 2];
        }
        return false;
    }

    friend class CPPParser;

public:
    Lexer(Language lang, DriverFMan::File file, DriverFMan& fman, Arena& arena,
          IdentMap& infoTable, MacroInfo* baseFileMacro) noexcept :
        lang_(lang),
        original_(file),
        ptr_(file.file->memfile.data()),
        fman_(fman),
        arena_(arena),
        infoTable_(infoTable) {
        setKeywords();
        setFileMacro(file);
        setBaseFileMacro(baseFileMacro);
        setMacros();
    }

    void skipWhitespace();
    /// @brief Returns true if encountered a new line.
    bool skipWhitespacePreProcess();
    void skipLine();
    void skipMultiline();

    Token next();
    Token nextNoPreProcessingDirective();
    Token nextNoPreProcessing();
    bool nextNoPreProcessing(Token& to);
    bool nextNoPreProcessingForce(Token& to);

    /// @brief Returns physical line.
    unsigned long requestLine() {
        unsigned long ln = 1;
        const char* start = getCurrentFile().file->memfile.data();
        while(start != ptr_) {
            if(*start == '\n') ln++;
            start++;
        }
        return ln;
    }

    DriverFMan::File getCurrentFile() const noexcept {
        if(includeStack_.size()) return includeStack_.back().file_;
        return original_;
    }

    /// @brief Marked as constexpr as a hint.
    inr_useattr(always_inline) constexpr static inline void reusableSkip(
        const char*& ptr) noexcept {
        if(!*ptr) return;

        ptr++;

        while(*ptr == '\\' && *(ptr + 1) == '\n') {
            ptr += 2;
        }
    }

    char peek() const noexcept {
        const char* next = ptr_;
        reusableSkip(next);
        return *next;
    }

    inr_useattr(always_inline) inr_useattr(hot) void skip() noexcept {
        reusableSkip(ptr_);
    }

    /// @brief Goes forward if the current char is c.
    bool forward(char c) noexcept {
        if(*ptr_ == c) {
            skip();
            return true;
        }
        return false;
    }
    /// @brief Jumps over the next char if its c.
    bool jump(char c) noexcept {
        if(peek() == c) {
            skip();
            skip();
            return true;
        }
        return false;
    }

    void setMacrosBasedOnTriple(inr::Triple);
    void addMacroWithPredefOne(inr::sview);
    void setTypeMacros(const CData&);
};

inline TokenCharIterator& TokenCharIterator::operator++() noexcept {
    Lexer::reusableSkip(ptr_);

    return *this;
}

} // namespace inrcc

#endif // INRCC_LEXER_LEXER_H
