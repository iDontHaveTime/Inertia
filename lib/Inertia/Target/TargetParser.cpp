#include "Inertia/Target/TargetParser.hpp"
#include "Inertia/Lexer/LexerOutput.hpp"
#include "Inertia/Lexer/TokenExpect.hpp"
#include "Inertia/Lexer/TokenStream.hpp"
#include "Inertia/Lexer/TokenType.hpp"
#include "Inertia/Mem/Archmem.hpp"
#include "Inertia/Target/TargetFile.hpp"
#include "Inertia/Target/TargetKeywords.hpp"
#include "Inertia/Target/TargetOutput.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace Inertia{

struct TargetParserCTX{
    TargetOutput tout;
    TokenStream ss;
    std::unordered_map<std::string_view, TargetParserType> lookup;
    bool cppinj;

    TargetParserCTX() = delete;
    TargetParserCTX(const TargetFile& tfile) noexcept : tout(tfile){};
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

    ctx.tout.target = ctx.ss.current().view_str();
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

uintmax_t get_token_value(TargetParserCTX& ctx){
    return std::stoull(ctx.ss.current().view_str(), nullptr, GetTypeBase(ctx.ss.current().type));
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
        std::string_view r = ctx.ss.current().view();
        ctx.tout.regclasses.push_back(r);
        ctx.lookup[r] = TargetParserType::REGCLASS;
        consume(ctx);
    }
    consume(ctx);

    return false;
}

bool ParseExtension(TargetParserCTX& ctx){
    consume(ctx);

    if(expect(TokenType::LeftSquare, ctx.ss) == expecterr::SUCCESS){
        consume(ctx);
    }
    else{
        return true;
    }

    while(expect(TokenType::RightSquare, ctx.ss) != expecterr::SUCCESS){
        if(!SaveableString(ctx.ss.current().type)) return true;
        ExtensionEntry entry;
        entry.name = ctx.ss.current().view();
        ctx.lookup[entry.name] = TargetParserType::EXTENSION;
        ctx.tout.extensions.push_back(entry);
        consume(ctx);
    }
    consume(ctx);

    return false;
}

// register NAME {class, width} // also parent
bool ParseRegister(TargetParserCTX& ctx){
    consume(ctx);

    RegisterEntry newEntry;

    newEntry.name = ctx.ss.current().view();

    consume(ctx);

    if(expect(TokenType::LeftBrace, ctx.ss) == expecterr::SUCCESS){
        consume(ctx);
    }
    else{
        return true;
    }

    while(1){
        if(ctx.ss.eof()) return true;
        if(expect(TokenType::RightBrace, ctx.ss) == expecterr::SUCCESS){
            consume(ctx);
            ctx.lookup[newEntry.name] = TargetParserType::REGISTER;
            ctx.tout.registers.push_back(std::move(newEntry));
            break;
        }
        else if(expect((int)TargetKeyword::WIDTH, ctx.ss) == expecterr::SUCCESS){
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

            newEntry.width = get_token_value(ctx);
            consume(ctx);
        }
        else if(expect((int)TargetKeyword::PARENT, ctx.ss) == expecterr::SUCCESS){
            consume(ctx);
            if(!SaveableString(ctx.ss.current().type)){
                return true;
            }
            newEntry.parent = ctx.ss.current().view();
            consume(ctx);
        }
        else if(expect((int)TargetKeyword::CLASS, ctx.ss) == expecterr::SUCCESS){
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

            std::string_view tofind = ctx.ss.current().view();
            auto it = ctx.lookup.find(tofind);

            if(it == ctx.lookup.end()){
                return true;
            }

            if(it->second != TargetParserType::REGCLASS){
                return true;
            }

            newEntry.classname = tofind;
            
            consume(ctx);
        }
        else if(expect((int)TargetKeyword::INIT, ctx.ss) == expecterr::SUCCESS){
            consume(ctx);

            if(expect(TokenType::LeftBrace, ctx.ss) == expecterr::SUCCESS){
                consume(ctx);
            }
            else{
                return true;
            }

            if(!ctx.cppinj){
                while(expect(TokenType::RightBrace, ctx.ss) != expecterr::SUCCESS){
                    if(ctx.ss.eof()) return true;
                    consume(ctx);
                }
                consume(ctx);
            }
            else{
                while(expect(TokenType::RightBrace, ctx.ss) != expecterr::SUCCESS){
                    if(ctx.ss.eof()) return true;
                    newEntry.init.push_back(ctx.ss.current().view_str());
                    consume(ctx);
                }
                consume(ctx);
            }
        }
        else{
            auto it = ctx.lookup.find(ctx.ss.current().view());
            if(it == ctx.lookup.end()){
                consume(ctx);
                continue;
            }
            if(it->second == TargetParserType::DATAENT){
                size_t datai;
                const DataEntry* dfound;
                for(size_t i = 0; i < ctx.tout.datas.size(); i++){
                    if(ctx.tout.datas[i].name == it->first){
                        newEntry.dataIndeces.push_back(i);
                        dfound = &ctx.tout.datas[i];
                        datai = i;
                        break;
                    }
                }
                consume(ctx);
                if(expect(TokenType::LeftParen, ctx.ss) == expecterr::SUCCESS){
                    consume(ctx);
                    while(1){
                        if(expect(TokenType::RightParen, ctx.ss) == expecterr::SUCCESS){
                            break;
                        }
                        else if(expect(TokenType::Star, ctx.ss) == expecterr::SUCCESS){
                            newEntry.inits.push_back({.init = false, .val = 0, .di = datai});
                            consume(ctx);
                        }
                        else{
                            if(NumberString(ctx.ss.current().type)){
                                if(newEntry.inits.size() >= dfound->data.size()) continue;
                                newEntry.inits.push_back({.init = true, .val = get_token_value(ctx), .di = datai});
                            }
                            consume(ctx);
                        }
                    }
                }
            }
            else{
                consume(ctx);
            }
        }
    }
    
    return false;
}

// __cpp_inc__ "HEADER" optionally <>
bool ParseCPPINC(TargetParserCTX& ctx){
    consume(ctx);
    if(!ctx.cppinj) return true;

    TokenType tt = ctx.ss.current().type;

    if(tt != TokenType::CharLiteral && tt != TokenType::StringLiteral) return true;

    std::string n = ctx.ss.current().view_str();

    consume(ctx);

    tt = ctx.ss.current().type;
    if(tt != TokenType::Left){
        ctx.tout.cppinc.emplace_back(std::move(n), CPPInclude::CPPIncludeType::Quotes);
        return false;
    }

    consume(ctx);
    ctx.tout.cppinc.emplace_back(std::move(n), CPPInclude::CPPIncludeType::Arrows);

    tt = ctx.ss.current().type;
    if(tt == TokenType::Right){
        consume(ctx);
    }

    return false;
}

// data NAME {}
bool ParseData(TargetParserCTX& ctx){
    consume(ctx);

    if(!SaveableString(ctx.ss.current().type)) return {};

    std::string_view name;

    if(expect_next(TokenType::LeftBrace, ctx.ss) != expecterr::SUCCESS){
        return true;
    }
    else{
        name = ctx.ss.current().view();
        ctx.lookup[name] = TargetParserType::DATAENT;
        consume(ctx);
    }

    consume(ctx);

    DataEntry newData;
    newData.name = name;

    while(1){
        if(ctx.ss.eof()) return true;
        if(expect(TokenType::RightBrace, ctx.ss) == expecterr::SUCCESS){
            consume(ctx);
            ctx.tout.datas.push_back(newData);
            return false;
        }
        else if(expect((int)TargetKeyword::BIT, ctx.ss) == expecterr::SUCCESS){
            consume(ctx);
            Data dt;
            dt.type = DataType::BIT;
            dt.had_default = false;
            if(expect(TokenType::Left, ctx.ss) == expecterr::SUCCESS){
                consume(ctx);
                if(!NumberString(ctx.ss.current().type)){
                    continue;
                }
                dt.width = get_token_value(ctx);
                if(dt.width > 64) dt.width = 64;
                consume(ctx);
                if(expect(TokenType::Right, ctx.ss) == expecterr::SUCCESS){
                    consume(ctx);
                }
                else{
                    continue;
                }
            }
            else{
                dt.width = 1;
            }

            if(!SaveableString(ctx.ss.current().type)){
                continue;
            }

            dt.name = ctx.ss.current().view();
            ctx.lookup[dt.name] = TargetParserType::DATAFIELD;
            consume(ctx);

            if(expect(TokenType::Equals, ctx.ss) == expecterr::SUCCESS){
                dt.had_default = true;
                consume(ctx);
                if(!NumberString(ctx.ss.current().type)) continue;
                dt.def_init = get_token_value(ctx);
                consume(ctx);
            }

            newData.data.push_back(dt);
        }
        else{
            consume(ctx);
        }
    }

    return true;
}

// instr NAME(ARGS...){...}
/* EXAMPLES:
instr mul(rax, rdx, GPR64 src){
    ...
}
*/

bool ParseInstruction(TargetParserCTX& ctx){
    consume(ctx);
    
    if(!SaveableString(ctx.ss.current().type)){
        return true;
    }

    std::string_view name = ctx.ss.current().view();

    InstructionEntry& entry = ctx.tout.instructions.emplace_back(name);
    ctx.lookup[name] = TargetParserType::INSTRUCTION;
    consume(ctx);

    if(expect(TokenType::LeftParen, ctx.ss) == expecterr::SUCCESS){
        consume(ctx);
    }
    else{
        return true;
    }

    while(1){
        if(ctx.ss.eof()) return true;
        if(expect(TokenType::RightParen, ctx.ss) == expecterr::SUCCESS){
            consume(ctx);
            break;
        }
        else{
            name = ctx.ss.current().view();
            auto it = ctx.lookup.find(name);
            switch(it->second){
                case TargetParserType::REGCLASS:
                    break;
                case TargetParserType::REGISTER:
                    entry.ops.push_back({
                        .name = name,
                        .type = it->second
                    });
                    break;
                case TargetParserType::DATAENT: // these are left for future
                    [[fallthrough]];
                case TargetParserType::DATAFIELD:
                    [[fallthrough]];
                case TargetParserType::INSTRUCTION:
                    [[fallthrough]];
                case TargetParserType::EXTENSION:
                    return true;
            }
        }
    }

    return false;
}

TargetOutput TargetParser::parse(std::vector<LexerOutput*> files){
    if(files.empty()) return {file};
    TargetParserCTX ctx(file);
    ctx.cppinj = cpp_injections;

    for(LexerOutput* lout : files){
        ctx.ss.open(lout);
        while(!ctx.ss.eof()){
            TokenType tt = ctx.ss.current().type;
    
            if(tt == TokenType::Keyword){
                switch((TargetKeyword)ctx.ss.current().getKeyword()){
                    case TargetKeyword::EXTENSION:
                        if(ParseExtension(ctx)){
                            return ctx.tout;
                        }
                        break;
                    case TargetKeyword::INSTRUCTION:
                        if(ParseInstruction(ctx)){
                            return ctx.tout;
                        }
                        break;
                    case TargetKeyword::DATA:
                        if(ParseData(ctx)){
                            return ctx.tout;
                        }
                        break;
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
                    case TargetKeyword::CPPINC:
                        if(ParseCPPINC(ctx)){
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
    }

    return ctx.tout;
}

}