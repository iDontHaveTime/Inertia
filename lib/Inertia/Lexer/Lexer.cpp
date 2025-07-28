#include "Inertia/Lexer/Lexer.hpp"
#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/LexerOutput.hpp"
#include "Inertia/Lexer/TokenType.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
#include <string_view>
#include <sys/types.h>
#include <thread>

#if defined(__GNUC__) || defined(__clang__)
    #define HOT_FUNC __attribute__((hot))
#else
    #define HOT_FUNC
#endif

using namespace Inertia;

enum class LexerState :uint8_t{
    Normal,
    Symbol,
    Alpha,
    Number,
    String,
};

enum class CommentType : uint8_t{
    Line,
    Block,
};


struct alignas(64) LexerContext{
    const char* cur;
    const char* const end;
    size_t tokStart = 0, tokEnd = 0, line = 1;
    LexerState state = LexerState::Normal;
    CommentType com = CommentType::Line;
    bool incom = false;
    bool inescape = false;
    TokenType look = TokenType::Special;
    TokenType type = TokenType::Special;
    std::string_view multiline_start, multiline_end, linecom;
    const Lexer* lex;
    short utf = 0;
    
    LexerContext(const char* s, const char* e) noexcept : cur(s), end(e){};
    LexerContext(const char* s, const char* e, const std::string& lcom, const std::string& mlstr, const std::string& mlend, const Lexer* lxr) : 
        cur(s), end(e), multiline_start(mlstr), multiline_end(mlend), linecom(lcom), lex(lxr){};
    LexerContext(const char* s, const char* e, const std::string_view& lcom, const std::string_view& mlstr, const std::string_view& mlend, const Lexer* lxr) : 
        cur(s), end(e), multiline_start(mlstr), multiline_end(mlend), linecom(lcom), lex(lxr){};
};

inline bool PeekableForward(LexerContext& ctx) noexcept{
    return (ctx.cur + 1 < ctx.end);
}

inline const char* PeekForward(LexerContext& ctx) noexcept{
    return (ctx.cur + 1);
}

inline void GoBack(LexerContext& ctx) noexcept{
    ctx.tokEnd--;
    ctx.cur--;
}

inline void EatToken(LexerContext& ctx, LexerOutput& out, TokenBuild& build){
    if(build.index == 0) return;

    bool isMultilineStart = (build == ctx.multiline_start);
    bool isLineComment = (build == ctx.linecom && ctx.com == CommentType::Line);
    bool isMultilineEnd = (build == ctx.multiline_end);

    if(isMultilineStart){
        ctx.incom = true;
        ctx.com = CommentType::Block;
    }
    else if(isLineComment){
        ctx.incom = true;
    }

    if(!ctx.incom){
        if(ctx.lex->usekwd){
            auto it = ctx.lex->keywords.find(build);
            if(it != ctx.lex->keywords.end()){
                out.push(ctx.tokStart, ctx.tokEnd, TokenType::Keyword, ctx.line, it->second);
                ctx.tokStart = ctx.tokEnd;
                build.clear();
                return;
            }
        }
        out.push(ctx.tokStart, ctx.tokEnd, ctx.type, ctx.line);
    }

    if(isMultilineEnd){
        ctx.incom = false;
        ctx.com = CommentType::Line;
    }

    ctx.tokStart = ctx.tokEnd;
    build.clear();
}

inline void SpecialCharacter(LexerContext& ctx) noexcept{
    if(*ctx.cur == '\n'){
        ctx.line++; 
        if(ctx.com == CommentType::Line) ctx.incom = false;
    }
}

inline void SwitchBasedOnType(LexerContext& ctx, TokenType type){
    switch(type){
        case TokenType::Special:
            SpecialCharacter(ctx);
            ctx.state = LexerState::Normal;
            break;
        case TokenType::UTF_Continue:
            std::cerr<<"Error during lexing UTF on line: "<<ctx.line<<std::endl;
            break;
        case TokenType::UTF_2:
            [[fallthrough]];
        case TokenType::UTF_3:
            [[fallthrough]];
        case TokenType::UTF_4:
            [[fallthrough]];
        case TokenType::Alpha:
            ctx.type = TokenType::Alpha;
            ctx.state = LexerState::Alpha;
            break;
        case TokenType::Number:
            ctx.type = TokenType::IntegerLiteral;
            ctx.state = LexerState::Number;
            break;
        case TokenType::DoubleQuote:
            ctx.type = TokenType::StringLiteral;
            ctx.state = LexerState::String;
            break;
        case TokenType::Quote:
            ctx.type = TokenType::CharLiteral;
            ctx.state = LexerState::String;
            break;
        default:
            // symbols
            ctx.type = ctx.look;
            ctx.state = LexerState::Symbol;
            break;
    }
}

inline void SwitchBasedOnCurrent(LexerContext& ctx){
    SwitchBasedOnType(ctx, ctx.look);
}

inline void CallBasedOnState(LexerContext& ctx, LexerOutput& out, TokenBuild& build);

inline void NormalState(LexerContext& ctx, LexerOutput& out, TokenBuild& build){
    SwitchBasedOnCurrent(ctx);
    ctx.tokStart = ctx.tokEnd;
    /*
    if(ctx.type == TokenType::CharLiteral){
        ctx.tokStart++;
    }
    */
    if(ctx.state != LexerState::Normal && ctx.state != LexerState::String){
        CallBasedOnState(ctx, out, build);
    }
}

HOT_FUNC inline void AlphaState(LexerContext& ctx, LexerOutput& out, TokenBuild& build){
    char c = *ctx.cur;
    if(ctx.look == Inertia::TokenType::Alpha){
        build.add_char(c);
        return;
    }

    if(ctx.look == TokenType::Number){
        build.add_char(c);
        return;
    }

    if(ctx.look == TokenType::UTF_2){
        if(ctx.utf != 0){
            std::cerr<<"Unknown UTF 2 byte sequence on line: "<<ctx.line<<std::endl;
            return;
        }
        ctx.utf = 1;
        build.add_char(c);
        return;
    }

    if(ctx.look == TokenType::UTF_3){
        if(ctx.utf != 0){
            std::cerr<<"Unknown UTF 3 byte sequence on line: "<<ctx.line<<std::endl;
            return;
        }
        ctx.utf = 2;
        build.add_char(c);
        return;
    }

    if(ctx.look == TokenType::UTF_4){
        if(ctx.utf != 0){
            std::cerr<<"Unknown UTF 4 byte sequence on line: "<<ctx.line<<std::endl;
            return;
        }
        ctx.utf = 3;
        build.add_char(c);
        return;
    }

    if(ctx.look == TokenType::UTF_Continue){
        if(ctx.utf == 0){
            std::cerr<<"Unknown UTF continuation sequence on line: "<<ctx.line<<std::endl;
            return;
        }
        ctx.utf--;
        build.add_char(c);
        return;
    }

    if(ctx.look == TokenType::Special){
        ctx.state = LexerState::Normal;
        EatToken(ctx, out, build);
        SpecialCharacter(ctx);
        return;
    }

    if(ctx.look == Inertia::TokenType::Quote){
        EatToken(ctx, out, build);
        ctx.type = Inertia::TokenType::CharLiteral;
        ctx.state = LexerState::String;
        return;
    }

    if(ctx.look == Inertia::TokenType::DoubleQuote){
        EatToken(ctx, out, build);
        ctx.type = Inertia::TokenType::StringLiteral;
        ctx.state = LexerState::String;
        return;
    }

    EatToken(ctx, out, build);
    SwitchBasedOnCurrent(ctx);
    CallBasedOnState(ctx, out, build);
}

inline bool isTypeSymbol(TokenType t) noexcept{
    return !(t == TokenType::Alpha || t == TokenType::Number || t == TokenType::Special || t == TokenType::Quote || t == TokenType::DoubleQuote);
}

inline void EatSwitchCall(LexerContext& ctx, LexerOutput& out, TokenBuild& build){
    EatToken(ctx, out, build);
    SwitchBasedOnCurrent(ctx);
    CallBasedOnState(ctx, out, build);
}

inline void NormalEatSpecial(LexerContext& ctx, LexerOutput& out, TokenBuild& build){
    ctx.state = LexerState::Normal;
    EatToken(ctx, out, build);
    SpecialCharacter(ctx);
}

inline void GoForward(LexerContext& ctx) noexcept{
    ctx.tokEnd++;
    ctx.cur++;
}

inline void AddAndGoForward(LexerContext& ctx, TokenBuild& build) noexcept{
    build.add_char(*ctx.cur);
    ctx.tokEnd++;
}

inline void SymbolPushBack(LexerContext& ctx, LexerOutput& out, TokenBuild& build){
    // PLEASE FIX THIS! IT WORKS BUT NO IDEA HOW!?
    if(build.index == 1){
        EatToken(ctx, out, build);
        ctx.state = LexerState::Normal;
    }
    else if(build.index == 2){
        TokenType match = ctx.lex->match(build);
        if(match != Inertia::TokenType::Special){
            ctx.type = match;
        }
        else{
            GoBack(ctx);
            build.index--;
            ctx.type = ctx.lex->look(*ctx.cur);
            EatToken(ctx, out, build);
            GoForward(ctx);
            ctx.type = ctx.lex->look(*ctx.cur);
            build.add_char(*ctx.cur);
        }
        EatToken(ctx, out, build);
        ctx.state = LexerState::Normal;
    }
    else if(build.index == 3){
        TokenType match = ctx.lex->match(build);
        if(match != Inertia::TokenType::Special){
            ctx.type = match;
        }
        else{
            GoBack(ctx);
            build.index--;
            GoBack(ctx);
            build.index--;
            ctx.type = ctx.lex->look(*ctx.cur);
            EatToken(ctx, out, build);
            GoForward(ctx);
            build.add_char(*ctx.cur);
            ctx.type = ctx.lex->look(*ctx.cur);
            EatToken(ctx, out, build);
            GoForward(ctx);
            ctx.type = ctx.lex->look(*ctx.cur);
            build.add_char(*ctx.cur);
        }
        EatToken(ctx, out, build);
        ctx.state = LexerState::Normal;
    }
}

inline void SymbolState(LexerContext& ctx, LexerOutput& out, TokenBuild& build){
    AddAndGoForward(ctx, build);

    if(PeekableForward(ctx)){
        const char* peek = PeekForward(ctx);
        TokenType pt = ctx.lex->look(*peek);
        
        if(build.index == 2){
            TokenType match = ctx.lex->match(build);
            if(match != Inertia::TokenType::Special){
                if(isTypeSymbol(pt)){
                    build.add_char(*peek);
                    TokenType match2 = ctx.lex->match(build);
                    if(match2 != Inertia::TokenType::Special){
                        ctx.type = match2;
                        ctx.tokEnd++;
                        EatToken(ctx, out, build);
                        ctx.tokEnd--;
                        ctx.state = LexerState::Normal;
                        ctx.cur++;
                        return;
                    }
                    else{
                        build.index--;
                    }
                }
                ctx.type = match;
                EatToken(ctx, out, build);
                ctx.state = LexerState::Normal;
                ctx.tokEnd--;
                return;
            }
        }
        if(!isTypeSymbol(pt) || build.index == 3){
            SymbolPushBack(ctx, out, build);
        }
    }
    else{
        SymbolPushBack(ctx, out, build);
    }
    ctx.tokEnd--;
}

inline bool IsUTF(TokenType t) noexcept{
    return (t == Inertia::TokenType::UTF_2 || t == Inertia::TokenType::UTF_3 || t == Inertia::TokenType::UTF_4);
}

inline short GetUTF(TokenType t) noexcept{
    if(t == Inertia::TokenType::UTF_2) return 1;
    if(t == Inertia::TokenType::UTF_3) return 2;
    if(t == Inertia::TokenType::UTF_4) return 3;
    return 0;
}

inline void NumberState(LexerContext& ctx, LexerOutput& out, TokenBuild& build){
    char c = *ctx.cur;
    if(ctx.look == TokenType::Number){
        build.add_char(c);
        return;
    }

    if(ctx.look == TokenType::Dot && ctx.type == TokenType::IntegerLiteral){
        ctx.type = TokenType::FloatLiteral;
        build.add_char(c);
        return;
    }
    
    if(ctx.look == TokenType::Alpha && ctx.type == TokenType::IntegerLiteral && build.index == 1){
        // hex and binary
        if(c == 'x' || c == 'X'){
            ctx.type = TokenType::HexLiteral;
            build.add_char(c);
            return;
        }
        if(c == 'b' || c == 'B'){
            ctx.type = TokenType::BinaryLiteral;
            build.add_char(c);
            return;
        }
        return;
        // if its not x or b, uhh, something happens, skip i think..?
    }

    if(ctx.look == TokenType::Alpha && ctx.type == TokenType::HexLiteral){
        if((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')){
            build.add_char(c);
            return;
        }
        return;
        // if its more than A/a and lesseq than F/f and if its not, idk
    }

    if(ctx.look == TokenType::DoubleQuote){
        EatToken(ctx, out, build);
        ctx.type = TokenType::StringLiteral;
        ctx.state = LexerState::String;
        return;
        // make this more modular come on
    }

    if(ctx.look == TokenType::Quote){
        EatToken(ctx, out, build);
        ctx.type = TokenType::CharLiteral;
        ctx.state = LexerState::String;
        return;
    }

    if(ctx.look == TokenType::Special){
        NormalEatSpecial(ctx, out, build);
        return;
    }

    if(IsUTF(ctx.look)){
        ctx.utf = GetUTF(ctx.look);
        ctx.state = LexerState::Alpha;
        return;
    }
    
    EatSwitchCall(ctx, out, build);
    // such a strange name
}

inline void EatString(LexerContext& ctx, LexerOutput& out, TokenBuild& build){
    //if(ctx.build.index > 0){
    if(!ctx.incom){
        out.push(ctx.tokStart, ctx.tokEnd, ctx.type, ctx.line, build);
    }
    ctx.tokStart = ctx.tokEnd;
    build.clear();
    //}
}

inline void EndOfLexing(LexerContext& ctx, LexerOutput& out, TokenBuild& build){
    EatToken(ctx, out, build);
}

inline void StringState(LexerContext& ctx, LexerOutput& out, TokenBuild& build){
    // TODO: Improve error handling in the whole lexer
    if(ctx.incom){
        ctx.state = LexerState::Normal;
        return;
    }

    char c = *ctx.cur;
    
    if((ctx.look == TokenType::Backslash || ctx.inescape) && ctx.utf == 0){
        if(ctx.inescape){
            build.add_char(ctx.lex->escape(c));
            ctx.inescape = false;
        }
        else{
            ctx.inescape = true;
            return;
        }
    }
    else if(ctx.look == TokenType::Quote && ctx.type == TokenType::CharLiteral && ctx.utf == 0){
        EatString(ctx, out, build);
        ctx.state = LexerState::Normal;
    }
    else if(ctx.look == TokenType::DoubleQuote && ctx.type == TokenType::StringLiteral && ctx.utf == 0){
        EatString(ctx, out, build);
        ctx.state = LexerState::Normal;
    }
    else{
        if(ctx.utf > 0){
            if(ctx.look != TokenType::UTF_Continue){
                std::cerr<<"Unknown UTF-8 sequence on line: "<<ctx.line<<std::endl;
                ctx.utf = 1;
            }
            build.add_char(c);
            ctx.utf--;
        }
        else{
            if((c & 0x80) == 0){ // ascii
                build.add_char(c);
            }
            else{
                if(ctx.look == TokenType::UTF_2){ // 110xxxxx (2 byte sequence)
                    ctx.utf = 1;
                    build.add_char(c);
                } 
                else if(ctx.look == TokenType::UTF_3){ // 1110xxxx (3 byte sequence)
                    ctx.utf = 2;
                    build.add_char(c);
                } 
                else if(ctx.look == TokenType::UTF_4){ // 11110xxx (4 byte sequence)
                    ctx.utf = 3;
                    build.add_char(c);
                }
                else if(ctx.look == TokenType::UTF_Continue){ // 10xxxxxx (continuation byte, but ctx.utf was 0)
                    std::cerr<<"Unknown UTF-8 sequence on line: "<<ctx.line<<std::endl;
                    build.add_char(c);
                }
                else{
                    std::cerr<<"Invalid UTF-8 leading byte on line: "<<ctx.line<<std::endl;
                    build.add_char(c);
                }
            }
        }
    }
}

HOT_FUNC inline void CallBasedOnState(LexerContext& ctx, LexerOutput& out, TokenBuild& build){
    switch(ctx.state){
        case LexerState::Normal:
            NormalState(ctx, out, build);
            return;
        case LexerState::Alpha:
            AlphaState(ctx, out, build);
            return;
        case LexerState::Symbol:
            SymbolState(ctx, out, build);
            return;
        case LexerState::String:
            StringState(ctx, out, build);
            return;
        case LexerState::Number:
            NumberState(ctx, out, build);
            return;
    }
}

size_t Lexer::assume(const LexerFile& file) const noexcept{
    const char* cur = file.raw();
    const char* end = file.fend();

    size_t assumed = 0;

    while(cur != end){
        if(this->issymbol(*cur)){
            assumed++;
        }
        cur++;
    }
    return assumed;
}

size_t Lexer::find_split(const LexerFile& file) const{
    return find_split(file.raw(), file.fend());
}

enum class ScannerState{
    Normal,
    Comment,
    String,
    Char
};

struct ScannerContext{
    size_t recSplit; // recommended split
    size_t actSplit = 0; // actual split
    const char* start;
    const char* cur;
    const char* end;
    const Lexer* lex;
    ScannerState state = ScannerState::Normal;
    TokenType look;
    bool done = false;
    CommentType com = CommentType::Line;
    bool escape = false;
};

inline void NormalScanner(ScannerContext& ctx){
    if(ctx.cur + ctx.lex->line_comment.length() < ctx.end && memcmp(ctx.cur, ctx.lex->line_comment.c_str(), ctx.lex->line_comment.length()) == 0){
        ctx.state = ScannerState::Comment;
        ctx.com = CommentType::Line;
    }
    else if(ctx.cur + ctx.lex->multiline_start.length() < ctx.end && memcmp(ctx.cur, ctx.lex->multiline_start.c_str(), ctx.lex->multiline_start.length()) == 0){
        ctx.state = ScannerState::Comment;
        ctx.com = CommentType::Block;
    }
    else{
        // not a comment
        if(ctx.look == Inertia::TokenType::DoubleQuote){
            ctx.state = ScannerState::String;
        }
        else if(ctx.look == Inertia::TokenType::Quote){
            ctx.state = ScannerState::Char;
        }
        else if(ctx.look == Inertia::TokenType::Special && *ctx.cur != '\n'){
            size_t offset = ctx.cur - ctx.start;
            if(offset >= ctx.recSplit){
                ctx.actSplit = offset;
                ctx.done = true;
            }
        }
    }
}

inline void CommentScanner(ScannerContext& ctx){
    if(ctx.com == CommentType::Line){
        if(*ctx.cur == '\n') ctx.state = ScannerState::Normal;
    }
    else{
        if(ctx.cur + ctx.lex->multiline_end.length() < ctx.end && memcmp(ctx.cur, ctx.lex->multiline_end.c_str(), ctx.lex->multiline_end.length()) == 0){
            ctx.cur += ctx.lex->multiline_end.length() - 1;
            // because it switches state it needs to be -1
            ctx.state = ScannerState::Normal;
        }
    }
}

inline void StringScanner(ScannerContext& ctx) noexcept{
    if(ctx.escape){
        ctx.escape = false;
    }
    else if(ctx.look == Inertia::TokenType::Backslash){
        ctx.escape = true;
    }
    else if(ctx.look == Inertia::TokenType::DoubleQuote){
        ctx.state = ScannerState::Normal;
    }
}

inline void CharScanner(ScannerContext& ctx) noexcept{
    if(ctx.escape){
        ctx.escape = false;
    }
    else if(ctx.look == Inertia::TokenType::Backslash){
        ctx.escape = true;
    }
    else if(ctx.look == Inertia::TokenType::Quote){
        ctx.state = ScannerState::Normal;
    }
}

size_t Lexer::find_split(const char* start, const char* end) const{
    if(!start || !end){
        std::cout<<"Start or end on splitting file not found"<<std::endl;
        return 0;
    }

    size_t length = end - start;

    if(length <= 1){
        std::cout<<"File is too short for splitting"<<std::endl;
        return 0;
    }

    ScannerContext ctx;
    ctx.lex = this;
    ctx.cur = start;
    ctx.end = end;
    ctx.start = start;
    ctx.recSplit = length>>1;

    while(ctx.cur != ctx.end){
        ctx.look = look(*ctx.cur);

        switch(ctx.state){
            case ScannerState::Normal:
                NormalScanner(ctx);
                break;
            case ScannerState::String:
                StringScanner(ctx);
                break;
            case ScannerState::Comment:
                CommentScanner(ctx);
                break;
            case ScannerState::Char:
                CharScanner(ctx);
                break;
        }

        if(ctx.done) return ctx.actSplit;
        ctx.cur++;
    }

    return ctx.actSplit;
}

LexerOutput Lexer::split_lex(const LexerFile* file){
    bool res;
    auto [sp1, sp2] = file->split(find_split(*file), &res);
    if(!res){
        std::cerr<<"Error splitting file in split_lex"<<std::endl;
        return {file};
    }

    return lex_2chunk(sp1, sp2, file);
}

LexerOutput Lexer::lex_chunk(const LexerFileChunk& chunk) const{
    LexerOutput out;
    TokenBuild build;
    out.file = nullptr;
    LexerContext ctx(chunk.raw(), chunk.raw() + chunk.len(), this->line_comment, this->multiline_start, this->multiline_end, this);
    
    while(ctx.cur != ctx.end){
        ctx.look = look(*ctx.cur);

        CallBasedOnState(ctx, out, build);

        ctx.cur++; ctx.tokEnd++;
    }

    EndOfLexing(ctx, out, build);

    return out;
}

std::vector<Token> Lexer::merge(std::vector<Token>& toks1, std::vector<Token>& toks2){
    std::vector<Token> out;
    out.reserve(toks1.size() + toks2.size());

    size_t line_offset = toks1.empty() ? 0 : (toks1.back().line - 1);

    for(Token& tok : toks2){
        tok.line += line_offset;
    }

    out.insert(out.end(), std::make_move_iterator(toks1.begin()), std::make_move_iterator(toks1.end()));
    out.insert(out.end(), std::make_move_iterator(toks2.begin()), std::make_move_iterator(toks2.end()));
    return out;
}

LexerOutput Lexer::merge_output(LexerOutput&& out1, LexerOutput&& out2){
    if(!out1.tokens.empty() && !out2.tokens.empty()){
        size_t line_offset = out1.tokens.back().line - 1;
        for(Token& tok : out2.tokens){
            tok.line += line_offset;
        }
    }

    out1.tokens.reserve(out1.tokens.size() + out2.tokens.size());
    out1.tokens.insert(out1.tokens.end(),
        std::make_move_iterator(out2.tokens.begin()), 
        std::make_move_iterator(out2.tokens.end()));

    return std::move(out1);
}

LexerOutput Lexer::lex_2chunk(const LexerFileChunk& chunk1, const LexerFileChunk& chunk2, const LexerFile* file){
    LexerOutput out1, out2;

    std::thread t1([&](){
        out1 = lex_chunk(chunk1);
    });

    std::thread t2([&](){
        out2 = lex_chunk(chunk2);
    });

    t1.join();
    t2.join();

    LexerOutput fout; 
    fout.file = file;

    return merge_output(std::move(out1), std::move(out2));
}

LexerOutput Lexer::lex_perf(const LexerFile& file, size_t assumed) const{
    LexerOutput out;
    TokenBuild build;
    out.tokens.reserve(assumed);
    out.file = &file;
    LexerContext ctx(file.raw(), file.fend(), this->line_comment, this->multiline_start, this->multiline_end, this);
    
    while(ctx.cur != ctx.end){
        ctx.look = look(*ctx.cur);

        CallBasedOnState(ctx, out, build);

        ctx.cur++; ctx.tokEnd++;
    }

    EndOfLexing(ctx, out, build);

    return out;
}

LexerOutput Lexer::lex(const LexerFile& file) const{
    LexerOutput out;
    TokenBuild build;
    out.file = &file;
    LexerContext ctx(file.raw(), file.fend(), this->line_comment, this->multiline_start, this->multiline_end, this);
    
    while(ctx.cur != ctx.end){
        ctx.look = look(*ctx.cur);

        CallBasedOnState(ctx, out, build);

        ctx.cur++; ctx.tokEnd++;
    }

    EndOfLexing(ctx, out, build);

    return out;
}