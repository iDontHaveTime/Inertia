#include "Inertia/IR/IRParser.hpp"
#include "Inertia/IR/Frame.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/IR/IRKeywords.hpp"
#include "Inertia/IR/IRNode.hpp"
#include "Inertia/IR/Instruction.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Lexer/LexerFile.hpp"
#include "Inertia/Lexer/LexerToken.hpp"
#include "Inertia/Lexer/TokenStream.hpp"
#include "Inertia/Lexer/TokenType.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include <cstddef>
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
    const LexerFile* file;
    Function* under;
    ArenaAlloc* allocator;
    expectgroup<IRKeyword, 8> IRTypes = {
        IRKeyword::I8, IRKeyword::I16, IRKeyword::I32, IRKeyword::I64,
        IRKeyword::F32, IRKeyword::F64, IRKeyword::PTR, IRKeyword::VOID
    };
    expectgroup<TokenType, 5> IRLiterals = {
        TokenType::CharLiteral, TokenType::IntegerLiteral, TokenType::BinaryLiteral,
        TokenType::HexLiteral, TokenType::FloatLiteral,
    };
    IRKeyword kwd = IRKeyword::NONE;
    TokenType tok_type;

    ParserContext() = default;
    ParserContext(IRParser* parser) noexcept{
        file = parser->get_file();
        frame.file = file;
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

bool ParseArgument(TokenStream& ss, ParserContext& ctx, TypeAllocator& talloc, Function& func){
    ArenaReference<Argument> newArg = ctx.allocator->alloc<Argument>();
    newArg->type = ParseType(ss, ctx, talloc);
    if(!newArg->type){
        return true;
    }

    if(expect(TokenType::Percent, ss) == expecterr::SUCCESS){
        consume(ss);
    }
    else{
        return true;
    }
    
    newArg->name = ss.current().view(*ctx.file);
    func.ssa[ss.current().view(*ctx.file)] = newArg.cast<IRNode>();
    func.args.push_back(newArg);

    consume(ss);

    return false;
}

bool ParseFunction(TokenStream& ss, ParserContext& ctx, TypeAllocator& talloc){
    consume(ss);

    Function& f = ctx.frame.funcs.emplace_back();
    f.instructions.set_arena(ctx.allocator);
    f.args.set_arena(ctx.allocator);
    
    f.retType = ParseType(ss, ctx, talloc);
    if(!f.retType) return true;

    if(expect(TokenType::At, ss) == expecterr::SUCCESS){
        consume(ss);

        f.name = ss.current().view(*ctx.file);
        consume(ss);
    }
    else{
        return true;
    }
    

    if(expect(TokenType::LeftParen, ss) == expecterr::SUCCESS){
        consume(ss);
        if(expect(TokenType::RightParen, ss) == expecterr::SUCCESS){
            consume(ss);
            return false;
        }
        else{
            while(true){
                if(ParseArgument(ss, ctx, talloc, f)){
                    return true;
                }

                if(expect(TokenType::RightParen, ss) == expecterr::SUCCESS){
                    consume(ss);
                    return false;
                }
                else if(expect(TokenType::Comma, ss) != expecterr::SUCCESS){
                    return true;
                }
                else{
                    consume(ss);
                }
            }
        }
    }
    else{
        return true;
    }

    return false;
}

ArenaReference<LiteralNode> ParseLiteral(TokenStream& ss, ParserContext& ctx, ArenaReference<Type>& tp){
    ArenaReference<LiteralNode> node = ctx.allocator->alloc<LiteralNode>();
    TokenType tt = ss.current().type;
    node->type = tp;
    std::string val;
    bool neg = false;
    if(expect(TokenType::Minus, ss) == expecterr::SUCCESS){
        if(tt == TokenType::FloatLiteral) val += '-';
        neg = true;
        consume(ss);
        tt = ss.current().type;
    }
    if(ctx.IRLiterals.expect(ss.current().type) != expecterr::SUCCESS){
        return {};
    }
    val += ss.current().view(*ctx.file);

    if(tt == TokenType::IntegerLiteral){
        node->value = std::stoull(val, nullptr, 10);
    }
    else if(tt == TokenType::BinaryLiteral){
        node->value = std::stoull(val, nullptr, 2);
    }
    else if(tt == TokenType::HexLiteral){
        node->value = std::stoull(val, nullptr, 16);
    }
    else if(tt == TokenType::FloatLiteral){
        bool skip = false;
        if(node->type == Type::FLOAT){
            FloatType* ft = (FloatType*)node->type.get();
            if(ft->accuracy == FloatType::FLOAT_ACC){
                float s = std::stof(val);
                node->value = *(float*)&s;
                skip = true;
            }
        }
        if(!skip){
            double s = std::stod(val);
            node->value = *(double*)&s;
        }
    }
    else if(tt == TokenType::CharLiteral){
        for(char c : val){
            node->value <<= (sizeof(char) * 8);
            node->value += c;
        }
    }

    if(neg && tt != TokenType::FloatLiteral){
        node->value = -(node->value);
    }
    consume(ss);

    return node;
}

ArenaReference<IRNode> GetSSAName(TokenStream& ss, ParserContext& ctx){
    return ctx.under->ssa[ss.current().view(*ctx.file)];
}

bool ParseReturn(TokenStream& ss, ParserContext& ctx, TypeAllocator& talloc){
    consume(ss);

    ReturnNode node;
    node.type = ParseType(ss, ctx, talloc);
    if(!node.type) return true;

    if(node.type->getKind() == Type::VOID){
        ctx.under->instructions.push_back_as<ReturnNode>(node);
        return false;
    }

    if(expect(TokenType::Percent, ss) == expecterr::SUCCESS){
        consume(ss);
        node.node = GetSSAName(ss, ctx);
        consume(ss);
    }
    else{
        auto lit = ParseLiteral(ss, ctx, node.type);
        node.node = lit.cast<IRNode>();
    }
    ctx.under->instructions.push_back_as<ReturnNode>(node);

    return false;
}

ArenaReference<BinaryOPNode> ParseBinaryOP(TokenStream& ss, ParserContext& ctx, TypeAllocator& talloc){
    ArenaReference<BinaryOPNode> bop = ctx.allocator->alloc<BinaryOPNode>();

    switch((IRKeyword)ss.current().getKeyword()){
        case IRKeyword::ADD:
            bop->optype = BinaryOPNode::ADD;
            break;
        default:
            return {};
    }
    consume(ss);

    bop->type = ParseType(ss, ctx, talloc);
    if(!bop->type){
        return {};
    }

    if(expect(TokenType::Percent, ss) == expecterr::SUCCESS){
        consume(ss);
        bop->lhs = GetSSAName(ss, ctx);
        consume(ss);
    }
    else{
        bop->lhs = ParseLiteral(ss, ctx, bop->type).cast<IRNode>();
    }

    if(expect(TokenType::Comma, ss) == expecterr::SUCCESS){
        consume(ss);
    }
    else{
        return {};
    }

    if(expect(TokenType::Percent, ss) == expecterr::SUCCESS){
        consume(ss);
        bop->rhs = GetSSAName(ss, ctx);
        consume(ss);
    }
    else{
        bop->rhs = ParseLiteral(ss, ctx, bop->type).cast<IRNode>();
    }

    return bop;
}

ArenaReference<IRNode> ParseInstruction(TokenStream& ss, ParserContext& ctx, TypeAllocator& talloc){

    switch((IRKeyword)ss.current().getKeyword()){
        case IRKeyword::ADD:
            return ParseBinaryOP(ss, ctx, talloc).cast<IRNode>();
        default:
            return {};
    }

    return {};
}

bool ParseSSA(TokenStream& ss, ParserContext& ctx, TypeAllocator& talloc){
    consume(ss);

    std::string_view name = ss.current().view(*ctx.file);
    consume(ss);

    if(expect(TokenType::Equals, ss) != expecterr::SUCCESS){
        return true;
    }

    consume(ss);

    ArenaReference<IRNode> ins = ParseInstruction(ss, ctx, talloc);
    if(!ins){
        return true;
    }

    ctx.under->ssa[name] = ins;
    // instruction

    return false;
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
                case IRKeyword::FUNCD:
                    if(ctx.under) break;
                    if(ParseFunction(ss, ctx, talloc)){
                        ctx.frame.funcs.pop_back();
                        std::cout<<"Error parsing function"<<std::endl;
                    }
                    break;
                case IRKeyword::RET:
                    if(!ctx.under) break;
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
                case TokenType::Percent:
                    if(!ctx.under) break;
                    if(ParseSSA(ss, ctx, talloc)){
                        std::cout<<"Error parsing ssa"<<std::endl;
                    }
                    break;
                case TokenType::LeftBrace:
                    consume(ss);
                    if(!ctx.under){
                        ctx.under = &ctx.frame.funcs.back();
                    }
                    else{
                        break;
                    }
                    break;
                case TokenType::RightBrace:
                    consume(ss);
                    if(ctx.under){
                        ctx.under = nullptr;
                    }
                    else{
                        break;
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


}