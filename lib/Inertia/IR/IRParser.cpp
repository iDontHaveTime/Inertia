#include "Inertia/IR/IRParser.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/IR/IRKeywords.hpp"
#include "Inertia/IR/Instruction.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/LexerToken.hpp"
#include "Inertia/Lexer/TokenStream.hpp"
#include "Inertia/Lexer/TokenType.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

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
    Function* under_function = nullptr;
    const LexerFile* file;
    ArenaAlloc* allocator;
    expectgroup<IRKeyword, 8> IRTypes = {
        IRKeyword::I8, IRKeyword::I16, IRKeyword::I32, IRKeyword::I64,
        IRKeyword::F32, IRKeyword::F64, IRKeyword::PTR, IRKeyword::VOID
    };
    IRKeyword kwd = IRKeyword::NONE;
    TokenType tok_type;
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
            case IRKeyword::F32:
                ref = talloc.getFloat(FloatType::FLOAT_ACC);
                break;
            case IRKeyword::F64:
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

Argument ParseArgument(TokenStream& ss, ParserContext& ctx, TypeAllocator& talloc){
    Argument newArg;
    newArg.type = ParseType(ss, ctx, talloc);
    if(!newArg.type) return {};

    if(expect(TokenType::Percent, ss) == expecterr::SUCCESS){
        consume(ss);
    }
    else{
        return {};
    }

    newArg.name = ss.current().view(*ctx.file);

    consume(ss);

    return newArg;
}

bool ParseFunction(TokenStream& ss, ParserContext& ctx, TypeAllocator& talloc){
    consume(ss);

    Function new_func;

    new_func.return_type = ParseType(ss, ctx, talloc);
    if(!new_func.return_type){
        return true;
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

    expecterr args = expect(TokenType::RightParen, ss);

    if(args == expecterr::SUCCESS){
        consume(ss);
    }
    else if(args == expecterr::FAILERR){
        // args (most likely)
        if(ctx.IRTypes.expect((IRKeyword)ss.current().getKeyword()) == expecterr::SUCCESS){
            while(1){
                new_func.args.push_back(ParseArgument(ss, ctx, talloc));
                if(!new_func.args.back().type){
                    return true;
                }
                if(expect(TokenType::RightParen, ss) == expecterr::SUCCESS){
                    consume(ss);
                    break;
                }
                else if(expect(TokenType::Comma, ss) == expecterr::SUCCESS){
                    consume(ss);
                }
                else{
                    return true;
                }
            }
        }
        else{
            return true;
        }

    }
    else{
        return true;
    }

    ctx.frame.functions.push_back(std::move(new_func));

    if(expect(TokenType::LeftBrace, ss) == expecterr::SUCCESS){
        ctx.under_function = &ctx.frame.functions.back();
    }
    else{
        // maybe flags
        return true;
    }

    return false;
}

bool ParseReturn(TokenStream& ss, ParserContext& ctx, TypeAllocator& talloc){
    consume(ss);
    ReturnInstruction ins;

    if(ctx.IRTypes.expect((IRKeyword)ss.current().getKeyword()) == expecterr::SUCCESS){
        ins.type = ParseType(ss, ctx, talloc);
        if(!ins.type) return true;
        if(ins.type->getKind() != Type::VOID){
            if(expect(TokenType::Percent, ss) == expecterr::SUCCESS){
                ins.ret_type = ReturnInstruction::SSA;
                consume(ss);
                ins.ret_val = ss.current().view(*ctx.file); // deal with it in codegen
                consume(ss);
            }
            else{
                if(expect(TokenType::Minus, ss) == expecterr::SUCCESS){
                    ins.negative = true;
                    consume(ss);
                }
                switch(ss.current().type){
                    case TokenType::HexLiteral:
                        ins.ret_type = ReturnInstruction::INTEGER;
                        break;
                    case TokenType::BinaryLiteral:
                        ins.ret_type = ReturnInstruction::INTEGER;
                        break;
                    case TokenType::IntegerLiteral:
                        ins.ret_type = ReturnInstruction::INTEGER;
                        break;
                    case TokenType::FloatLiteral:
                        ins.ret_type = ReturnInstruction::FLOAT;
                        break;
                    case TokenType::CharLiteral:
                        ins.ret_type = ReturnInstruction::INTEGER;
                        break;
                    default:
                        return true;
                }
                ins.ret_val = ss.current().view(*ctx.file);
                consume(ss);
            }
        }
    }
    else{
        return true;
    }

    if(expect(TokenType::Semicolon, ss) == expecterr::SUCCESS){
        consume(ss);
    }
    else{
        return true;
    }

    if(ctx.under_function){
        ctx.under_function->instructions.push_back((ctx.allocator->alloc<ReturnInstruction>(ins)));
    }
    else{
        return true;
    }

    return false;
}

Frame IRParser::parse_tokens(const LexerOutput& tokens, TypeAllocator& talloc){
    TokenStream ss(tokens);
    ParserContext ctx;
    ctx.file = file;
    ctx.allocator = &allocator;
    
    while(!ss.eof()){
        ctx.tok_type = ss.current().type;
        
        if(ctx.tok_type == TokenType::Keyword){
            ctx.kwd = (IRKeyword)ss.current().getKeyword();
            switch(ctx.kwd){
                case IRKeyword::FUNCD:
                    if(ctx.under_function){
                        break;
                    }
                    if(ParseFunction(ss, ctx, talloc)){
                        std::cout<<"Error parsing function"<<std::endl;
                    }
                    break;
                case IRKeyword::RET:
                    if(!ctx.under_function){
                        break;
                    }
                    if(ParseReturn(ss, ctx, talloc)){
                        std::cout<<"Error parsing return"<<std::endl;
                    }
                    break;
                default:
                    consume(ss);
                    break;
            }
        }
        else{
            switch(ctx.tok_type){
                case TokenType::RightBrace:
                    consume(ss);
                    if(ctx.under_function){
                        ctx.under_function = nullptr;
                    }
                    break;
                default:
                    consume(ss);
                    break;
            }
        }
    }

    return ctx.frame;
}

std::string IRParser::type_to_string(Type* t){
    if(!t) return "";
    std::string res;
    
    uint32_t ptrl = 0;
    while(t->getKind() == Type::POINTER){
        ptrl++;
        t = ((PointerType*)t)->pointee;
    }

    switch(t->getKind()){
        case Type::INTEGER:
            res += 'i';
            res += std::to_string(((IntegerType*)t)->width);
            break;
        case Type::FLOAT:
            res += 'f';
            switch(((FloatType*)t)->accuracy){
                case FloatType::FLOAT_ACC:
                    res += "32";
                    break;
                case FloatType::DOUBLE_ACC:
                    res += "64";
                    break;
            }
            break;
        case Type::VOID:
            res += "void";
            break;
        default:
            res += "unknown";
            break;
    }

    while(ptrl--){
        res += '*';
    }
    return res;
}

std::string IRParser::arg_to_string(Argument& arg){
    if(!arg.type) return "";
    std::string str = type_to_string(arg.type);
    
    str += " %";
    str += arg.name;

    return str;
}

std::string IRParser::function_to_string(Function& func){
    if(!func.return_type) return "";
    std::string str = "funcd ";
    str += type_to_string(func.return_type);

    str += " @";
    str += func.name;

    str += '(';

    for(Argument& arg : func.args){
        str += arg_to_string(arg);

        if(&arg != &func.args.back()){
            str += ", ";
        }
    }

    str += ')';

    if(func.instructions.size() > 0){
        str += "{\n";

        for(auto& ins : func.instructions){
            str += ' ';
            switch(ins->ins_type){
                case Instruction::RET:{
                        str += "ret ";
                        ReturnInstruction* ret = (ReturnInstruction*)ins.get();
                        str += type_to_string(ret->type);
                        str += ' ';
                        if(!ret){
                            break;
                        }
                        if(ret->negative){
                            str += '-';
                        }
                        if(ret->ret_type == ReturnInstruction::SSA){
                            str += '%';
                        }
                        str += ret->ret_val;
                    }
                    break;
            }
            str += '\n';
        }

        str += "}";
    }

    return str;
}


}