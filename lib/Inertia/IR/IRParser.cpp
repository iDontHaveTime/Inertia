#include "Inertia/IR/IRParser.hpp"
#include "Inertia/IR/Frame.hpp"
#include "Inertia/IR/IRKeywords.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/LexerToken.hpp"
#include "Inertia/Lexer/TokenStream.hpp"
#include "Inertia/Lexer/TokenType.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include "Inertia/Lexer/TokenExpect.hpp"
#include <cstddef>

namespace Inertia{

struct ParserContext{
    Frame frame;
    const LexerFile* file;
    ArenaAlloc* allocator;
    expectgroup<IRKeyword, 8> IRTypes = {
        IRKeyword::I8, IRKeyword::I16, IRKeyword::I32, IRKeyword::I64,
        IRKeyword::FLOAT, IRKeyword::DOUBLE, IRKeyword::PTR, IRKeyword::VOID
    };
    expectgroup<TokenType, 5> IRLiterals = {
        TokenType::CharLiteral, TokenType::IntegerLiteral, TokenType::BinaryLiteral,
        TokenType::HexLiteral, TokenType::FloatLiteral
    };
    IRKeyword kwd = IRKeyword::NONE;
    TokenType tok_type;

    ParserContext() = default;
    ParserContext(IRParser* parser) noexcept{
        file = parser->get_file();
        allocator = &parser->get_allocator();
    }

    ~ParserContext() = default;
};

inline void consume(TokenStream& ss) noexcept{
    ss<<1;
}

ArenaReference<Type> ParseType(TokenStream& ss, ParserContext& ctx, TypeAllocator& talloc){
    ArenaReference<Type> ref;
    if(ctx.IRTypes.expect((IRKeyword)ss.current().getKeyword()) == expecterr::SUCCESS){
        switch((IRKeyword)ss.current().getKeyword()){
            case IRKeyword::I8:
                ref = talloc.getInteger(8);
                break;
            case IRKeyword::I16:
                ref = talloc.getInteger(16);
                break;
            case IRKeyword::I32:
                ref = talloc.getInteger(32);
                break;
            case IRKeyword::I64:
                ref = talloc.getInteger(64);
                break;
            case IRKeyword::FLOAT:
                ref = talloc.getFloat(FloatType::FLOAT_ACC);
                break;
            case IRKeyword::DOUBLE:
                ref = talloc.getFloat(FloatType::DOUBLE_ACC);
                break;
            case IRKeyword::VOID:
                ref = talloc.getVoid();
                break;
            default:
                return {};
        }
    } 
    else{
        return {};
    }
    consume(ss);

    while(ss.current().type == TokenType::Star){
        ref = talloc.getPointer(ref.get());
        consume(ss);
    }


    return ref;
}

Frame IRParser::parse_tokens(const LexerOutput& tokens, TypeAllocator& talloc){
    if(!file) return {};
    TokenStream ss(tokens);
    ParserContext ctx(this);

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

    return ctx.frame;
}


}