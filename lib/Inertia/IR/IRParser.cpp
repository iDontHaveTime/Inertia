#include "Inertia/IR/IRParser.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/IR/IRKeywords.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/LexerToken.hpp"
#include "Inertia/Lexer/TokenStream.hpp"
#include "Inertia/Lexer/TokenType.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include <cstddef>

namespace Inertia{

enum class expecterr{
    SUCCESS = 0, FAILERR = 1, EOFERR = -1
};

template<typename T, size_t n>
struct expectgroup{
    T group[n];

    template<typename... Args>
    expectgroup(Args... args){
        static_assert(sizeof...(args) == n, "Argument count not equals to N");
        size_t i = 0;
        auto unpack_args = [&](auto&&... vals){
            ((group[i++] = vals), ...);
        };
        unpack_args(args...);
    }

    inline expecterr expect(T tt){
        for(size_t i = 0; i < n; i++){
            if(group[i] == tt){
                return expecterr::SUCCESS;
            }
        }
        return expecterr::FAILERR;
    }
};

expecterr expect_next(TokenType tt, TokenStream& ss) noexcept{
    if(!ss.peekablef()) return expecterr::EOFERR;
    return ss.peekf().type == tt ? expecterr::SUCCESS : expecterr::FAILERR;
}

expecterr expect_next(IRKeyword kwd, TokenStream& ss) noexcept{
    if(!ss.peekablef()) return expecterr::EOFERR;
    IRKeyword got = (IRKeyword)ss.peekf().getKeyword();
    if(got != kwd) return expecterr::FAILERR;
    return expecterr::SUCCESS;
}

expecterr expect(TokenType tt, TokenStream& ss) noexcept{
    TokenType got = ss.current().type;
    if(got == TokenType::TokenEOF) return expecterr::EOFERR;
    return got == tt ? expecterr::SUCCESS : expecterr::FAILERR;
}

expecterr expect(IRKeyword kwd, TokenStream& ss) noexcept{
    if(ss.current().type == TokenType::TokenEOF) return expecterr::EOFERR;
    IRKeyword got = (IRKeyword)ss.current().getKeyword();
    if(got != kwd) return expecterr::FAILERR;
    return expecterr::SUCCESS;
}

struct ParserContext{
    Frame frame;
    const LexerFile* file;
    expectgroup<IRKeyword, 8> IRTypes = {
        IRKeyword::I8, IRKeyword::I16, IRKeyword::I32, IRKeyword::I64,
        IRKeyword::F32, IRKeyword::F64, IRKeyword::PTR, IRKeyword::VOID
    };
    IRKeyword kwd = IRKeyword::NONE;
};

inline void consume(TokenStream& ss) noexcept{
    ss<<1;
}

bool ParseFunction(TokenStream& ss, ParserContext& ctx, TypeAllocator& talloc){
    consume(ss);

    Function new_func;

    if(ctx.IRTypes.expect((IRKeyword)ss.current().getKeyword()) == expecterr::SUCCESS){
        switch((IRKeyword)ss.current().getKeyword()){
                case IRKeyword::I8:
                    new_func.return_type = talloc.getInteger(8);
                    break;
                case IRKeyword::I16:
                    new_func.return_type = talloc.getInteger(16);
                    break;
                case IRKeyword::I32:
                    new_func.return_type = talloc.getInteger(32);
                    break;
                case IRKeyword::I64:
                    new_func.return_type = talloc.getInteger(64);
                    break;
                case IRKeyword::F32:
                    new_func.return_type = talloc.getFloat(FloatType::FLOAT_ACC);
                    break;
                case IRKeyword::F64:
                    new_func.return_type = talloc.getFloat(FloatType::DOUBLE_ACC);
                    break;
                case IRKeyword::VOID:
                    new_func.return_type = talloc.getVoid();
                    break;
                default:
                    return true;
            }
    } 
    else{
        return true;
    }
    consume(ss);

    while(ss.current().type == TokenType::Star){
        if(ss.current().type == TokenType::Star){
            new_func.return_type = talloc.getPointer(new_func.return_type.get());
        }
        consume(ss);
    }

    if(expect(TokenType::At, ss) == expecterr::SUCCESS){
        consume(ss);
    }
    else{
        return true;
    }

    if(expect(TokenType::Alpha, ss) == expecterr::SUCCESS){
        new_func.name = ss.current().view(*ctx.file);
        consume(ss);
    }
    else{
        return true;
    }

    if(expect(TokenType::LeftParen, ss) == expecterr::SUCCESS){
        consume(ss);
    }
    else{
        return true;
    }

    // args, soon

    if(expect(TokenType::RightParen, ss) == expecterr::SUCCESS){
        consume(ss);
    }
    else{
        return true;
    }

    ctx.frame.functions.push_back(std::move(new_func));

    return false;
}

Frame IRParser::parse_tokens(const LexerOutput& tokens, TypeAllocator& talloc){
    TokenStream ss(tokens);
    ParserContext ctx;
    ctx.file = file;
    
    while(!ss.eof()){
        ctx.kwd = (IRKeyword)ss.current().getKeyword();
        switch(ctx.kwd){
            case IRKeyword::FUNCD:
                ParseFunction(ss, ctx, talloc);
                break;
            default:
                consume(ss);
                break;
        }
    }

    return ctx.frame;
}

}