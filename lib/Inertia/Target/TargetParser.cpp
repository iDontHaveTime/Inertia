#include "Inertia/Target/TargetParser.hpp"
#include "Inertia/Lexer/LexerOutput.hpp"
#include "Inertia/Lexer/TokenExpect.hpp"
#include "Inertia/Lexer/TokenStream.hpp"
#include "Inertia/Lexer/TokenType.hpp"
#include "Inertia/Mem/Archmem.hpp"
#include "Inertia/Target/TargetFile.hpp"
#include "Inertia/Target/TargetKeywords.hpp"
#include "Inertia/Target/TargetOutput.hpp"
#include <string>
#include <unordered_map>

namespace Inertia{

enum class TargetParserType{
    REGCLASS,
    REGISTER
};

struct TargetParserCTX{
    TargetOutput tout;
    TokenStream ss;
    std::unordered_map<std::string_view, TargetParserType> lookup;

    TargetParserCTX() = delete;
    TargetParserCTX(const TargetFile& tfile, const LexerOutput& lout) noexcept : tout(tfile), ss(lout){};
};

inline void consume(TargetParserCTX& ctx) noexcept{
    ctx.ss<<1;
}

inline bool SaveableString(TokenType tt) noexcept{
    switch(tt){
        case TokenType::Alpha:
            [[fallthrough]];
        case TokenType::Number:
            [[fallthrough]];
        case TokenType::StringLiteral:
            [[fallthrough]];
        case TokenType::CharLiteral:
            [[fallthrough]];
        case TokenType::IntegerLiteral:
            [[fallthrough]];
        case TokenType::FloatLiteral:
            [[fallthrough]];
        case TokenType::HexLiteral:
            [[fallthrough]];
        case TokenType::BinaryLiteral:
            [[fallthrough]];
        case TokenType::Keyword:
            return true;
        default:
            return false;
    }
}

inline bool NumberString(TokenType tt) noexcept{
    switch(tt){
        case TokenType::IntegerLiteral:
            [[fallthrough]];
        case TokenType::HexLiteral:
            [[fallthrough]];
        case TokenType::BinaryLiteral:
            return true;
        default:
            return false;
    }
}

inline int GetTypeBase(TokenType tt) noexcept{
    switch(tt){
        case TokenType::IntegerLiteral:
            return 10;
        case TokenType::HexLiteral:
            return 16;
        case TokenType::BinaryLiteral:
            return 2;
        default:
            return -1;
    }
}

bool ParseTarget(TargetParserCTX& ctx) noexcept{
    if(!ctx.tout.target.empty()) return true;
    consume(ctx);

    if(expect(TokenType::Equals, ctx.ss) == expecterr::SUCCESS){
        consume(ctx);
    }
    else{
        return true;
    }

    if(!SaveableString(ctx.ss.current().type)) return true;

    ctx.tout.target = ctx.ss.current().view_str(*ctx.tout.file.lfile);
    consume(ctx);
    return false;
}

bool ParseEndian(TargetParserCTX& ctx) noexcept{
    if(ctx.tout.endian != Endian::IN_UNKNOWN_ENDIAN) return true;
    consume(ctx);

    if(expect(TokenType::Equals, ctx.ss) == expecterr::SUCCESS){
        consume(ctx);
    }
    else{
        return true;
    }

    TargetKeyword kwd = (TargetKeyword)ctx.ss.current().getKeyword();

    if(kwd == TargetKeyword::LITTLE){
        ctx.tout.endian = Endian::IN_LITTLE_ENDIAN;
    }
    else if(kwd == TargetKeyword::BIG){
        ctx.tout.endian = Endian::IN_BIG_ENDIAN;
    }
    else{
        return true;
    }

    consume(ctx);
    
    return false;
}

bool ParseRegclass(TargetParserCTX& ctx){
    consume(ctx);

    if(expect(TokenType::LeftSquare, ctx.ss) == expecterr::SUCCESS){
        consume(ctx);
    }
    else{
        return true;
    }

    while(expect(TokenType::RightSquare, ctx.ss) != expecterr::SUCCESS){
        if(!SaveableString(ctx.ss.current().type)) return true;
        std::string_view r = ctx.ss.current().view(*ctx.tout.file.lfile);
        ctx.tout.regclasses.push_back(r);
        ctx.lookup[r] = TargetParserType::REGCLASS;
        consume(ctx);
    }
    consume(ctx);

    return false;
}

bool ParseRegister(TargetParserCTX& ctx){
    consume(ctx);

    RegisterEntry newEntry;

    newEntry.name = ctx.ss.current().view(*ctx.tout.file.lfile);

    consume(ctx);

    if(expect(TokenType::LeftBrace, ctx.ss) == expecterr::SUCCESS){
        consume(ctx);
    }
    else{
        return true;
    }

    while(1){
        if(expect(TokenType::RightBrace, ctx.ss) == expecterr::SUCCESS){
            consume(ctx);
            ctx.lookup[newEntry.name] = TargetParserType::REGISTER;
            ctx.tout.registers.push_back(newEntry);
            break;
        }
        if(expect((int)TargetKeyword::WIDTH, ctx.ss) == expecterr::SUCCESS){
            consume(ctx);

            if(expect(TokenType::Equals, ctx.ss) == expecterr::SUCCESS){
                consume(ctx);
            }
            else{
                return true;
            }

            if(!NumberString(ctx.ss.current().type)){
                return true;
            }

            newEntry.width = std::stoi(ctx.ss.current().view_str(*ctx.tout.file.lfile), nullptr, GetTypeBase(ctx.ss.current().type));
            consume(ctx);
        }
        if(expect((int)TargetKeyword::PARENT, ctx.ss) == expecterr::SUCCESS){
            consume(ctx);
            if(!SaveableString(ctx.ss.current().type)){
                return true;
            }
            newEntry.parent = ctx.ss.current().view(*ctx.tout.file.lfile);
            consume(ctx);
        }
        if(expect((int)TargetKeyword::CLASS, ctx.ss) == expecterr::SUCCESS){
            consume(ctx);

            if(expect(TokenType::Equals, ctx.ss) == expecterr::SUCCESS){
                consume(ctx);
            }
            else{
                return true;
            }

            if(!SaveableString(ctx.ss.current().type)){
                return true;
            }

            std::string_view tofind = ctx.ss.current().view(*ctx.tout.file.lfile);
            auto it = ctx.lookup.find(tofind);

            if(it == ctx.lookup.end()){
                return true;
            }

            if(it->second != TargetParserType::REGCLASS){
                return true;
            }

            for(size_t i = 0; i < ctx.tout.regclasses.size(); i++){
                if(ctx.tout.regclasses[i] == tofind){
                    newEntry.classid = (int)i;
                    break;
                }
            }
            
            consume(ctx);
        }
    }
    
    return false;
}

TargetOutput TargetParser::parse(const LexerOutput& lout){
    TargetParserCTX ctx(file, lout);

    while(!ctx.ss.eof()){
        TokenType tt = ctx.ss.current().type;

        if(tt == TokenType::Keyword){
            switch((TargetKeyword)ctx.ss.current().getKeyword()){
                case TargetKeyword::TARGET:
                    if(ParseTarget(ctx)){
                        return ctx.tout;
                    }
                    break;
                case TargetKeyword::ENDIAN:
                    if(ParseEndian(ctx)){
                        return ctx.tout;
                    }
                    break;
                case TargetKeyword::REGCLASS:
                    if(ParseRegclass(ctx)){
                        return ctx.tout;
                    }
                    break;
                case TargetKeyword::REGISTER:
                    if(ParseRegister(ctx)){
                        return ctx.tout;
                    }
                    break;
                default:
                    consume(ctx);
                    break;
            }
        }
        else{
            switch(tt){
                default:
                    consume(ctx);
                    break;
            }
        }
    }

    return ctx.tout;
}

}