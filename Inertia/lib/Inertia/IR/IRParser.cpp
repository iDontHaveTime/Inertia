#include "Inertia/IR/IRParser.hpp"
#include "Inertia/IR/IRBuilder.hpp"
#include "Inertia/IR/IRKeywords.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/LexerToken.hpp"
#include "Inertia/Lexer/TokenStream.hpp"
#include "Inertia/Lexer/TokenType.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include "Inertia/Lexer/TokenExpect.hpp"
#include <cstddef>
#include <cstdint>

namespace Inertia{

struct ParserContext{
    const LexerFile* file;
    IRBuilder& builder;
    expectgroup<IRKeyword, 5> IRTypes = {
        IRKeyword::INT, IRKeyword::FLOAT, IRKeyword::DOUBLE,
        IRKeyword::PTR, IRKeyword::VOID
    };
    expectgroup<TokenType, 5> IRLiterals = {
        TokenType::CharLiteral, TokenType::IntegerLiteral, TokenType::BinaryLiteral,
        TokenType::HexLiteral, TokenType::FloatLiteral
    };
    IRKeyword kwd = IRKeyword::NONE;
    TokenType tok_type;

    ParserContext(IRParser* parser, IRBuilder& _builder) noexcept : builder(_builder){
        file = parser->get_file();
    }

    ~ParserContext() noexcept = default;
};

inline void consume(TokenStream& ss) noexcept{
    ss<<1;
}

uint8_t hexchar_to_val(char c) noexcept{
    if(c <= '9' && c >= '0') return c - '0';
    if(c <= 'F' && c >= 'A') return (c - 'A') + 10;
    if(c <= 'f' && c >= 'a') return (c - 'a') + 10;
    return (uint8_t)-1;
}

uintmax_t GetTokenValue(TokenStream& ss) noexcept{
    uintmax_t val = 0;
    std::string_view str = ss.current().view();
    if(str.empty()) return 0;
    switch(ss.current().type){
        case TokenType::CharLiteral:
            for(char c : str){
                val <<= 8;
                val += c;
            }
            break;
        case TokenType::IntegerLiteral:
            for(char c : str){
                val *= 10;
                val += c - '0';
            }
            break;
        case TokenType::HexLiteral:{
                size_t i = 0;
                if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) i += 2;
                if(i >= str.length()) return 0;
                for(; i < str.length(); i++){
                    val <<= 4;
                    val += hexchar_to_val(str[i]);
                }
            }
            break;
        case TokenType::BinaryLiteral:{
                size_t i = 0;
                if(str[0] == '0' && (str[1] == 'b' || str[1] == 'B')) i += 2;
                if(i >= str.length()) return 0;
                for(; i < str.length(); i++){
                    val <<= 1;
                    val += str[i] - '0';
                }
            }
            break;
        default: return 0;
    }
    return val;
}

ArenaReference<Type> ParseType(TokenStream& ss, ParserContext& ctx){
    ArenaReference<Type> ref;
    if(ctx.IRTypes.expect((IRKeyword)ss.current().getKeyword()) == expecterr::SUCCESS){
        switch((IRKeyword)ss.current().getKeyword()){
            case IRKeyword::INT:
                consume(ss);
                if(expect(TokenType::Left, ss) == expecterr::SUCCESS){
                    consume(ss);
                }
                else{
                    return {};
                }
                if(ctx.IRLiterals.expect(ss.current().type) == expecterr::SUCCESS){
                    ref = ctx.builder.getAllocator()->getInteger(GetTokenValue(ss));
                    consume(ss);
                }
                else{
                    return {};
                }
                if(expect(TokenType::Right, ss) == expecterr::SUCCESS){
                    consume(ss);
                }
                else{
                    return {};
                }
                break;
            case IRKeyword::FLOAT:
                ref = ctx.builder.getAllocator()->getFloat(FloatType::FLOAT_ACC);
                consume(ss);
                break;
            case IRKeyword::DOUBLE:
                ref = ctx.builder.getAllocator()->getFloat(FloatType::DOUBLE_ACC);
                consume(ss);
                break;
            case IRKeyword::VOID:
                ref = ctx.builder.getAllocator()->getVoid();
                consume(ss);
                break;
            default:
                return {};
        }
    }
    else{
        return {};
    }

    while(ss.current().type == TokenType::Star){
        ref = ctx.builder.getAllocator()->getPointer(ref);
        consume(ss);
    }


    return ref;
}

bool IRParser::parse_tokens(const LexerOutput& tokens, IRBuilder& builder){
    if(!file) return false;
    TokenStream ss(tokens);
    ParserContext ctx(this, builder);

    while(!ss.eof()){
        ctx.tok_type = ss.current().type;

        if(ctx.tok_type == TokenType::Keyword){
            ctx.kwd = (IRKeyword)ss.current().getKeyword();
            switch(ctx.kwd){
                default:
                    consume(ss);
                    break;
            }
        }
        else{
            switch(ctx.tok_type){
                default:
                    consume(ss);
                    break;
            }
        }
    }

    return false;
}


}
