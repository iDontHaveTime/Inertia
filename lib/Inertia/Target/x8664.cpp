#include "Inertia/Target/x8664.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/IR/IRNode.hpp"
#include "Inertia/IR/Instruction.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include "Inertia/Mem/Memstream.hpp"
#include "Inertia/Target/Target.hpp"
#include <cstdint>
#include <string>

namespace Inertia{

enum class general_widthx8664{
    X8, X16, X32, X64
};

struct saved_regx8664{
    ArenaReference<LowerRegisterx86_64> reg;
    general_widthx8664 w;
    saved_regx8664(ArenaReference<LowerRegisterx86_64>& r, general_widthx8664 wi) noexcept : reg(r), w(wi){};
};

struct func_ssa_context{
    ArenaReference<IRNode> node;
    ArenaReference<GeneralRegisterx86_64> reg;
    bool mem = false;
    long offset = 0;
};

struct func_context{
    std::vector<func_ssa_context> ssa_ctx;
    size_t stack = 8;
};

struct contextx8664{
    Targetx86_64* tg;
    std::vector<saved_regx8664> saved;
    bool destlast;
    bool synced = true;
};

MemoryStream& generate_registerx8664(MemoryStream& mss, ArenaReference<LowerRegisterx86_64> reg, contextx8664& ctx) noexcept{
    switch(ctx.tg->syntax){
        case TargetInfo::GNUAS:
            mss<<'%'<<reg->name;
            break;
        default:
            mss<<reg->name;
            break;
    }
    return mss;
}

MemoryStream& generate_registermemoffx8664(MemoryStream& mss, ArenaReference<LowerRegisterx86_64> reg, contextx8664& ctx, long off) noexcept{
    switch(ctx.tg->syntax){
        case TargetInfo::GNUAS:
            if(off > 0){
                mss<<std::to_string(off);
            }
            mss<<'('<<'%'<<reg->name<<')';
            break;
        default:
            mss<<'['<<reg->name;
            if(off != 0){
                if(off > 0){
                    mss<<'+';
                }
                mss<<std::to_string(off);
            }
            mss<<']';
            break;
    }
    return mss;
}

MemoryStream& generate_immediatex8664(MemoryStream& mss, intmax_t val, contextx8664& ctx) noexcept{
    switch(ctx.tg->syntax){
        case TargetInfo::GNUAS:
            mss<<'$'<<std::to_string(val);
            break;
        default:
            mss<<std::to_string(val);
            break;
    }
    return mss;
}

MemoryStream& generate_widthx8664(MemoryStream& mss, general_widthx8664 w, contextx8664& ctx) noexcept{
    switch(ctx.tg->syntax){
        case TargetInfo::GNUAS:
            switch(w){
                case general_widthx8664::X8:
                    mss<<'b';
                    break;
                case general_widthx8664::X16:
                    mss<<'w';
                    break;
                case general_widthx8664::X32:
                    mss<<'l';
                    break;
                case general_widthx8664::X64:
                    mss<<'q';
                    break;
            }
            break;
        default:
            break;
    }
    return mss;
}

MemoryStream& generate_pushx8664(MemoryStream& mss, general_widthx8664 w, contextx8664& ctx) noexcept{
    switch(ctx.tg->syntax){
        case TargetInfo::GNUAS:
            mss<<"push";
            generate_widthx8664(mss, w, ctx);
            break;
        default:
            mss<<"push";
            break;
    }
    return mss;
}

MemoryStream& generate_xorx8664(MemoryStream& mss, general_widthx8664 w, contextx8664& ctx) noexcept{
    switch(ctx.tg->syntax){
        case TargetInfo::GNUAS:
            mss<<"xor";
            generate_widthx8664(mss, w, ctx);
            break;
        default:
            mss<<"xor";
            break;
    }
    return mss;
}

MemoryStream& generate_movx8664(MemoryStream& mss, general_widthx8664 w, contextx8664& ctx) noexcept{
    switch(ctx.tg->syntax){
        case TargetInfo::GNUAS:
            mss<<"mov";
            generate_widthx8664(mss, w, ctx);
            break;
        default:
            mss<<"mov";
            break;
    }
    return mss;
}

MemoryStream& generate_popx8664(MemoryStream& mss, general_widthx8664 w, contextx8664& ctx) noexcept{
    switch(ctx.tg->syntax){
        case TargetInfo::GNUAS:
            mss<<"pop";
            generate_widthx8664(mss, w, ctx);
            break;
        default:
            mss<<"pop";
            break;
    }
    return mss;
}

MemoryStream& generate_retx8664(MemoryStream& mss, contextx8664& ctx) noexcept{
    switch(ctx.tg->syntax){
        case TargetInfo::GNUAS:
            mss<<"ret";
            break;
        default:
            mss<<"ret";
            break;
    }
    return mss;
}

void patch_stack_offsetsx8664(func_context& fctx, long by, contextx8664& ctx){
    for(func_ssa_context& fssa : fctx.ssa_ctx){
        if(fssa.reg != ctx.tg->abi.stackPointer) continue;
        fssa.offset += by;
    }
}

void increase_stack_x8664(func_context& fctx, long by, contextx8664& ctx){
    fctx.stack += by;
    patch_stack_offsetsx8664(fctx, by, ctx);
}

bool save_registerx8664(MemoryStream& mss, ArenaReference<LowerRegisterx86_64> reg, contextx8664& ctx, general_widthx8664 w, func_context& fctx) noexcept{
    generate_pushx8664(mss, w, ctx)<<' ';   
    generate_registerx8664(mss, reg, ctx);
    ctx.saved.emplace_back(reg, w);
    increase_stack_x8664(fctx, 8, ctx);
    return false;
}

bool unsave_registerx8664(MemoryStream& mss, ArenaReference<LowerRegisterx86_64> reg, contextx8664& ctx, general_widthx8664 w, func_context& fctx) noexcept{
    generate_popx8664(mss, w, ctx)<<' ';
    generate_registerx8664(mss, reg, ctx);
    increase_stack_x8664(fctx, -8, ctx);
    return false;
}

bool unsave_all_registersx8664(MemoryStream& mss, contextx8664& ctx, func_context& fctx) noexcept{
    for(auto it = ctx.saved.rbegin(); it != ctx.saved.rend(); it++){
        mss<<'\t';
        unsave_registerx8664(mss, it->reg, ctx, it->w, fctx);
        mss<<'\n';
    }
    return false;
}

bool generate_prologuex8664(MemoryStream& mss, contextx8664& ctx, func_context& fctx) noexcept{

    if(ctx.tg->abi.framePointer->flags & RegisterFlags::CALEE_SAVED){
        mss<<'\t';
        save_registerx8664(mss, ctx.tg->abi.framePointer->l64b, ctx, general_widthx8664::X64, fctx);
        mss<<'\n';
    }


    return false;
}

bool generate_epiloguex8664(MemoryStream& mss, contextx8664& ctx, func_context& fctx) noexcept{

    fctx.stack -= 8; // for return address

    if(ctx.tg->abi.framePointer->flags & RegisterFlags::CALEE_SAVED){
        unsave_all_registersx8664(mss, ctx, fctx);
    }


    return false;
}

MemoryStream& generate_globalx8664(MemoryStream& mss, contextx8664& ctx) noexcept{
    switch(ctx.tg->syntax){
        case TargetInfo::GNUAS:
            mss<<".globl";
            break;
        default:
            mss<<"global";
            break;
    }

    return mss;
}

void generate_alignx8664(MemoryStream& mss, int align, contextx8664& ctx) noexcept{
    switch(ctx.tg->syntax){
        case TargetInfo::GNUAS:
            mss<<".balign";
            break;
        default:
            mss<<"align";
            break;
    }
    mss<<' ';

    mss<<std::to_string(align);
}

MemoryStream& set_returnregister_ssa(MemoryStream& mss, contextx8664& ctx, func_ssa_context& fss, Type* it){
    
    if(it->getKind() != Type::FLOAT){
        generate_movx8664(mss, general_widthx8664::X64, ctx)<<' ';
        if(ctx.destlast){
            if(fss.mem){
                generate_registermemoffx8664(mss, fss.reg->l64b, ctx, fss.offset);
            }
            else{
                generate_registerx8664(mss, fss.reg->l64b, ctx);
            }
            mss<<", ";
            generate_registerx8664(mss, ctx.tg->abi.returnRegister->l64b, ctx);
        }
        else{
            generate_registerx8664(mss, ctx.tg->abi.returnRegister->l64b, ctx);
            mss<<", ";
            if(fss.mem){
                generate_registermemoffx8664(mss, fss.reg->l64b, ctx, fss.offset);
            }
            else{
                generate_registerx8664(mss, fss.reg->l64b, ctx);
            }
        }
    }
    
    return mss;
}

MemoryStream& set_returnregister_immx8664(MemoryStream& mss, contextx8664& ctx, intmax_t val, Type* it){
    if(it->getKind() == Type::INTEGER || it->getKind() == Type::POINTER){
        if(val == 0){
            generate_xorx8664(mss, general_widthx8664::X32, ctx)<<' ';
            generate_registerx8664(mss, ctx.tg->abi.returnRegister->l32b, ctx);
            mss<<','<<' ';
            generate_registerx8664(mss, ctx.tg->abi.returnRegister->l32b, ctx);
        }
        else{
            
            general_widthx8664 width = (val < INT32_MAX) ? general_widthx8664::X32 : general_widthx8664::X64;
            if(it->getKind() == Type::INTEGER){
                if(val < 0){
                    if(((IntegerType*)it)->width > 32){
                        width = general_widthx8664::X64;
                    }
                }
            }
            else{
                width = general_widthx8664::X64;
            }
            ArenaReference<LowerRegisterx86_64> reg = width == general_widthx8664::X32 ? ctx.tg->abi.returnRegister->l32b : ctx.tg->abi.returnRegister->l64b;
    
            generate_movx8664(mss, width, ctx)<<' ';
    
            if(ctx.destlast){
                generate_immediatex8664(mss, val, ctx)<<", ";
                generate_registerx8664(mss, reg, ctx);
            }
            else{
                generate_registerx8664(mss, reg, ctx)<<", ";
                generate_immediatex8664(mss, val, ctx);
            }
        }
    }
    return mss;
}

bool generate_returninstructionx8664(MemoryStream& mss, func_context& fctx, contextx8664& ctx, ReturnNode* node){
    generate_epiloguex8664(mss, ctx, fctx);
    
    if(node->type->getKind() != Type::VOID){
        mss<<'\t';
        if(node->node->node_type == IRNodeType::Literal){
            LiteralNode* lit = (LiteralNode*)node->node.get();
            set_returnregister_immx8664(mss, ctx, lit->value, lit->type.get());
        }
        else if(node->node->node_type == IRNodeType::Instruction){

        }
        else if(node->node->node_type == IRNodeType::Argument){
            func_ssa_context ssa;
            for(func_ssa_context& fss : fctx.ssa_ctx){
                if(fss.node == node->node){
                    ssa = fss;
                }
            }
            if(!ssa.reg){
                std::cout<<"Error codegen ssa return reg"<<std::endl;
                return true;
            }
            set_returnregister_ssa(mss, ctx, ssa, node->node->type);
        }
        mss<<'\n';
    }
    
    mss<<'\t';
    generate_retx8664(mss, ctx)<<'\n';

    return false;
}

bool generate_binaryopx8664(MemoryStream& mss, func_context& fctx, contextx8664& ctx, InstructionNode* node){

    return false;
}

bool generate_instructionx8664(MemoryStream& mss, func_context& fctx, contextx8664& ctx, InstructionNode* node){
    switch(node->ins_type){
        case InstructionType::BINARYOP:
            return generate_binaryopx8664(mss, fctx, ctx, node);
        case InstructionType::RET:
            return generate_returninstructionx8664(mss, fctx, ctx, (ReturnNode*)node);
        default:
            return true;
    }
}

bool address_args(func_ssa_context& fctx, size_t argc, contextx8664& ctx, func_context& func){
    if(argc < ctx.tg->abi.reg_args.size()){
        fctx.reg = ctx.tg->abi.reg_args[argc];
    }
    else{
        fctx.mem = true;
        fctx.reg = ctx.tg->abi.stackPointer;
        fctx.offset = (func.stack + (8 * (argc - ctx.tg->abi.reg_args.size())));
    }
    return false;
}

bool generate_functionx8664(MemoryStream& mss, const Function& func, contextx8664& ctx){
    mss<<'\t';
    generate_globalx8664(mss, ctx)<<' '<<func.name<<'\n'<<'\t';

    generate_alignx8664(mss, ctx.tg->abi.alignFunctions, ctx);
    mss<<'\n';

    mss<<func.name<<':'<<'\n';
    func_context fctx;

    
    for(const ArenaReference<Argument>& arg : func.args){
        func_ssa_context newctx;
        newctx.node = arg.cast<IRNode>();
        auto it = func.ssa.find(arg->name);
        if(it == func.ssa.end()) return true;
        newctx.node = it->second;
        if(address_args(newctx, fctx.ssa_ctx.size(), ctx, fctx)){
            std::cout<<"Error addressing args"<<std::endl;
            return true;
        }
        fctx.ssa_ctx.push_back(newctx);
    }

    for(const ArenaReference<IRNode>& node : func.instructions){
        if(node->node_type == IRNodeType::Instruction){
            if(generate_instructionx8664(mss, fctx, ctx, (InstructionNode*)node.get())){
                std::cout<<"Error codegen instruction"<<std::endl;
                return true;
            }
        }
        else{
            return true;
        }
    }

    return false;
}

bool generate_sectionx8664(MemoryStream& mss, const char* section, contextx8664& ctx) noexcept{
    switch(ctx.tg->syntax){
        case TargetInfo::GNUAS:
            mss<<".section";
            break;
        default:
            mss<<"section";
            break;
    }

    mss<<' '<<section<<'\n';

    return false;
}

bool Targetx86_64::generate_assembly(MemoryStream& mss, const Frame& frame, TargetInfo* info){
    if(!mss || !info || info->target != x86_64) return true;
    contextx8664 ctx;
    ctx.tg = (Targetx86_64*)info;
    ctx.destlast = (ctx.tg->syntax == GNUAS);
    if(!ctx.tg->regs) return true;

    // md

    if(generate_sectionx8664(mss, ctx.tg->execSection, ctx)) return true;
    for(const Function& func : frame.funcs){
        if(generate_functionx8664(mss, func, ctx)){
            std::cout<<"Error codegen func"<<std::endl;
        }
    }

    return false;
}

}