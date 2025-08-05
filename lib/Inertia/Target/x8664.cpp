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
    X8 = 8, X16 = 16, X32 = 32, X64 = 64
};

struct saved_regx8664{
    ArenaReference<LowerRegisterx86_64> reg;
    general_widthx8664 w;
    saved_regx8664(ArenaReference<LowerRegisterx86_64> r, general_widthx8664 wi) noexcept : reg(r), w(wi){};
};

struct func_ssa_context{
    ArenaReference<IRNode> node;
    ArenaReference<GeneralRegisterx86_64> reg;
    bool mem = false;
    long offset = 0;
};

struct func_context{
    std::vector<func_ssa_context> ssa_ctx;
    std::vector<saved_regx8664> saved;
    std::vector<saved_regx8664> to_save;
    size_t subplaceholder = 0;
    size_t stack = 8;
    size_t startStack = 8;
    int args_on_stack_alloced = 0;
};

struct contextx8664{
    Targetx86_64* tg;
    bool destlast;
    bool synced = true;
};

func_ssa_context* find_ssa_x8664(func_context& fctx, ArenaReference<IRNode> node){
    for(func_ssa_context& ssa : fctx.ssa_ctx){
        if(ssa.node == node) return &ssa;
    }
    return nullptr;
}

bool register_used_x8664(func_context& fctx, ArenaReference<GeneralRegisterx86_64> reg, contextx8664& ctx){
    if(reg == ctx.tg->abi.stackPointer) return true;
    if(reg == ctx.tg->abi.framePointer && ctx.tg->useFramePointer) return true;
    for(func_ssa_context& ssa : fctx.ssa_ctx){
        if(ssa.reg == reg) return true;
    }
    return false;
}

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
            if(off != 0){
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
            switch(w){
                case general_widthx8664::X8:
                    mss<<" byte";
                    break;
                case general_widthx8664::X16:
                    mss<<" word";
                    break;
                case general_widthx8664::X32:
                    mss<<" dword";
                    break;
                case general_widthx8664::X64:
                    mss<<" qword";
                    break;
            }
            break;
    }
    return mss;
}

MemoryStream& generate_gmnemonicx8664(const char* mnem, MemoryStream& mss, general_widthx8664 w, contextx8664& ctx, bool cw) noexcept{
    mss<<mnem;
    if(cw) generate_widthx8664(mss, w, ctx);
    return mss;
}

MemoryStream& generate_pushx8664(MemoryStream& mss, general_widthx8664 w, contextx8664& ctx) noexcept{
    generate_gmnemonicx8664("push", mss, w, ctx, true);
    return mss;
}

MemoryStream& generate_xorx8664(MemoryStream& mss, general_widthx8664 w, contextx8664& ctx) noexcept{
    generate_gmnemonicx8664("xor", mss, w, ctx, true);
    return mss;
}

MemoryStream& generate_movx8664(MemoryStream& mss, general_widthx8664 w, contextx8664& ctx) noexcept{
    generate_gmnemonicx8664("mov", mss, w, ctx, true);
    return mss;
}

MemoryStream& generate_popx8664(MemoryStream& mss, general_widthx8664 w, contextx8664& ctx) noexcept{
    generate_gmnemonicx8664("pop", mss, w, ctx, true);
    return mss;
}

MemoryStream& generate_retx8664(MemoryStream& mss, contextx8664& ctx) noexcept{
    generate_gmnemonicx8664("ret", mss, general_widthx8664::X64, ctx, false);
    return mss;
}

void patch_stack_offsetsx8664(func_context& fctx, long by, contextx8664& ctx){
    for(func_ssa_context& fssa : fctx.ssa_ctx){
        if(fssa.reg != ctx.tg->abi.stackPointer) continue;
        fssa.offset += by;
    }
}

void increase_stack_x8664(func_context& fctx, long by, contextx8664& ctx){
    patch_stack_offsetsx8664(fctx, by, ctx);
    fctx.stack += by;
}

bool save_registerx8664(MemoryStream& mss, ArenaReference<LowerRegisterx86_64> reg, contextx8664& ctx, general_widthx8664 w, func_context& fctx) noexcept{
    for(auto& rs : fctx.saved){
        if(rs.reg == reg) return false;
    }
    generate_pushx8664(mss, w, ctx)<<' ';   
    generate_registerx8664(mss, reg, ctx);
    fctx.saved.emplace_back(reg, w);
    increase_stack_x8664(fctx, 8, ctx);
    fctx.startStack += 8;
    return false;
}

bool unsave_register_epiloguex8664(MemoryStream& mss, ArenaReference<LowerRegisterx86_64> reg, contextx8664& ctx, general_widthx8664 w) noexcept{
    generate_popx8664(mss, w, ctx)<<' ';
    generate_registerx8664(mss, reg, ctx);
    
    return false;
}

bool unsave_registerx8664(MemoryStream& mss, ArenaReference<LowerRegisterx86_64> reg, contextx8664& ctx, general_widthx8664 w, func_context& fctx) noexcept{
    unsave_register_epiloguex8664(mss, reg, ctx, w);
    increase_stack_x8664(fctx, -8, ctx);
    fctx.startStack -= 8;
    return false;
}

bool unsave_all_registersx8664(MemoryStream& mss, contextx8664& ctx, func_context& fctx) noexcept{
    for(auto it = fctx.saved.rbegin(); it != fctx.saved.rend(); it++){
        mss<<'\t';
        unsave_register_epiloguex8664(mss, it->reg, ctx, it->w);
        mss<<'\n';
    }
    return false;
}

bool function_ep_stackallocx8664(MemoryStream& mss, contextx8664& ctx, func_context& fctx){
    if(fctx.stack <= fctx.startStack) return false;
    size_t save = mss.size();
    mss.seek(fctx.subplaceholder);
    generate_gmnemonicx8664("sub", mss, general_widthx8664::X64, ctx, true)<<' ';
    if(ctx.destlast){
        generate_immediatex8664(mss, fctx.stack - fctx.startStack, ctx);
        mss<<", ";
        generate_registerx8664(mss, ctx.tg->abi.stackPointer->l64b, ctx);
    }
    else{
        generate_registerx8664(mss, ctx.tg->abi.stackPointer->l64b, ctx);
        mss<<", ";
        generate_immediatex8664(mss, fctx.stack - fctx.startStack, ctx);
    }
    mss.seek(save);
    return false;
}

bool generate_prologuex8664(MemoryStream& mss, contextx8664& ctx, func_context& fctx) noexcept{

    if(ctx.tg->useFramePointer){
        if(ctx.tg->abi.framePointer->flags & RegisterFlags::CALEE_SAVED){
            mss<<'\t';
            save_registerx8664(mss, ctx.tg->abi.framePointer->l64b, ctx, general_widthx8664::X64, fctx);
            mss<<'\n';
        }
        else{
            fctx.to_save.emplace_back(ctx.tg->abi.framePointer->l64b, general_widthx8664::X64);
        }
        mss<<'\t';
        generate_movx8664(mss, general_widthx8664::X64, ctx)<<' ';
    
        if(ctx.destlast){
            generate_registerx8664(mss, ctx.tg->abi.stackPointer->l64b, ctx);
            mss<<", ";
            generate_registerx8664(mss, ctx.tg->abi.framePointer->l64b, ctx);
        }
        else{
            generate_registerx8664(mss, ctx.tg->abi.framePointer->l64b, ctx);
            mss<<", ";
            generate_registerx8664(mss, ctx.tg->abi.stackPointer->l64b, ctx);
        }
    
        mss<<'\n';
    }

    mss<<'\t';
    fctx.subplaceholder = mss.size();
    long placeholder = 40;
    while(placeholder--){
        mss<<' ';
    }
    mss<<'\n';


    return false;
}

bool generate_epiloguex8664(MemoryStream& mss, contextx8664& ctx, func_context& fctx) noexcept{

    if(fctx.stack > fctx.startStack){
        mss<<'\t';
        if(ctx.tg->useFramePointer){
            generate_movx8664(mss, general_widthx8664::X64, ctx)<<' ';
            if(ctx.destlast){
                generate_registerx8664(mss, ctx.tg->abi.framePointer->l64b, ctx);
                mss<<", ";
                generate_registerx8664(mss, ctx.tg->abi.stackPointer->l64b, ctx);
            }
            else{
                generate_registerx8664(mss, ctx.tg->abi.stackPointer->l64b, ctx);
                mss<<", ";
                generate_registerx8664(mss, ctx.tg->abi.framePointer->l64b, ctx);
            }
        }
        else{
            generate_gmnemonicx8664("add", mss, general_widthx8664::X64, ctx, true)<<' ';
            if(ctx.destlast){
                generate_immediatex8664(mss, fctx.stack - fctx.startStack, ctx);
                mss<<", ";
                generate_registerx8664(mss, ctx.tg->abi.stackPointer->l64b, ctx);
            }
            else{
                generate_registerx8664(mss, ctx.tg->abi.stackPointer->l64b, ctx);
                mss<<", ";
                generate_immediatex8664(mss, fctx.stack - fctx.startStack, ctx);
            }
        }
        mss<<'\n';
    }

    if(ctx.tg->abi.framePointer->flags & RegisterFlags::CALEE_SAVED){
        unsave_all_registersx8664(mss, ctx, fctx);
    }


    return false;
}

bool allocate_stackx8664(func_context& fctx, long by, contextx8664& ctx){
    increase_stack_x8664(fctx, by, ctx);
    return false;
}

ArenaReference<LowerRegisterx86_64> return_lowerreg_width(general_widthx8664 w, ArenaReference<GeneralRegisterx86_64> reg){
    switch(w){
        case general_widthx8664::X8:
            return reg->l8b;
        case general_widthx8664::X16:
            return reg->l16b;
        case general_widthx8664::X32:
            return reg->l32b;
        case general_widthx8664::X64:
            return reg->l64b;
    default: return {};
    }
}

bool spill_to_stackx8664(MemoryStream& mss, func_ssa_context& ssa, func_context& fctx, contextx8664& ctx, long size, long align, long base){
    if(ssa.mem == true) return true;
    func_ssa_context newssa;
    newssa.mem = true;
    newssa.reg = ctx.tg->useFramePointer ? ctx.tg->abi.framePointer : ctx.tg->abi.stackPointer;
    if((fctx.stack & (align - 1)) != 0){
        allocate_stackx8664(fctx, (((fctx.stack + align - 1) & ~(align - 1)) - fctx.stack), ctx);
    }
    if(ctx.tg->useFramePointer){
        allocate_stackx8664(fctx, size, ctx);
        newssa.offset = (long)(fctx.startStack - fctx.stack);
    }
    else{
        newssa.offset = (long)(fctx.stack - base);
        allocate_stackx8664(fctx, size, ctx);
    }
    newssa.node = ssa.node;

    if(ssa.mem == false && ssa.reg){
        if(ssa.node->type->getKind() == Type::INTEGER || ssa.node->type->getKind() == Type::POINTER){
            general_widthx8664 width = general_widthx8664::X64;
            if(ssa.node->type->getKind() == Type::INTEGER){
                IntegerType* it = (IntegerType*)ssa.node->type.get();
                width = (general_widthx8664)it->width;
            }

            mss<<'\t';
            generate_movx8664(mss, width, ctx)<<' ';
            if(ctx.destlast){
                generate_registerx8664(mss, return_lowerreg_width(width, ssa.reg), ctx);
                mss<<", ";
                generate_registermemoffx8664(mss, newssa.reg->l64b, ctx, newssa.offset);
            }
            else{
                generate_registermemoffx8664(mss, newssa.reg->l64b, ctx, newssa.offset);
                mss<<", ";
                generate_registerx8664(mss, return_lowerreg_width(width, ssa.reg), ctx);
            }
            mss<<'\n';
        }
    }

    ssa = newssa;
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

func_ssa_context* generate_instructionx8664(MemoryStream& mss, func_context& fctx, contextx8664& ctx, ArenaReference<InstructionNode> node);

// return not used, so returning (void*)1 means success
func_ssa_context* generate_returninstructionx8664(MemoryStream& mss, func_context& fctx, contextx8664& ctx, ArenaReference<ReturnNode> node){
    if(node->type->getKind() != Type::VOID){
        if(node->node->node_type == IRNodeType::Literal){
            mss<<'\t';
            LiteralNode* lit = (LiteralNode*)node->node.get();
            set_returnregister_immx8664(mss, ctx, lit->value, lit->type.get());
            mss<<'\n';
        }
        else if(node->node->node_type == IRNodeType::Instruction){
            func_ssa_context* ins = find_ssa_x8664(fctx, node->node);
            if(!ins){
                ins = generate_instructionx8664(mss, fctx, ctx, node->node.cast<InstructionNode>());
            }
            if(!ins) return nullptr;
            if(ins->mem != false || ins->reg != ctx.tg->abi.returnRegister){
                mss<<'\t';
                set_returnregister_ssa(mss, ctx, *ins, ins->node->type);
                mss<<'\n';
            }
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
                return nullptr;
            }
            mss<<'\t';
            set_returnregister_ssa(mss, ctx, ssa, node->node->type);
            mss<<'\n';
        }
    }

    generate_epiloguex8664(mss, ctx, fctx);
    
    mss<<'\t';
    generate_retx8664(mss, ctx)<<'\n';

    return ((func_ssa_context*)1);
}

func_ssa_context allocate_ssa_x8664(MemoryStream& mss, func_context& fctx, contextx8664& ctx, ArenaReference<IRNode> for_node){
    func_ssa_context newssa;
    newssa.node = for_node;

    for(const ArenaReference<GeneralRegisterx86_64>& reg : ctx.tg->abi.usableRegs){
        if(reg->flags & RegisterFlags::CALEE_SAVED){
            continue;
        }
        if(register_used_x8664(fctx, reg, ctx)){
            continue;
        }
        else{
            newssa.reg = reg;
            return newssa;
        }
    }

    if(!newssa.reg){
        spill_to_stackx8664(mss, newssa, fctx, ctx, 8, 8, fctx.startStack);
    }

    return newssa;
}

ArenaReference<GeneralRegisterx86_64> get_temporary_register(MemoryStream& mss, func_context& fctx, contextx8664& ctx){
    ArenaReference<GeneralRegisterx86_64> candidate;
    for(ArenaReference<GeneralRegisterx86_64>& reg : ctx.tg->abi.usableRegs){
        if(register_used_x8664(fctx, reg, ctx)){
            continue;
        }
        else{
            candidate = reg;
        }
    }
    if(!candidate){
        for(func_ssa_context& ssa : fctx.ssa_ctx){
            if(ssa.mem == false && ssa.reg){
                candidate = ssa.reg;
                spill_to_stackx8664(mss, ssa, fctx, ctx, 8, 8, fctx.startStack);
                return candidate;
            }
        }
    }
    else{
        if(candidate->flags & RegisterFlags::CALEE_SAVED){
            mss<<'\t';
            save_registerx8664(mss, candidate->l64b, ctx, general_widthx8664::X64, fctx);
            mss<<'\n';
        }
        return candidate;
    }
    return {};
}

MemoryStream& write_ssa_x8664(MemoryStream& mss, contextx8664& ctx, func_ssa_context& ssa, general_widthx8664 w){
    if(ssa.mem){
        generate_registermemoffx8664(mss, return_lowerreg_width(general_widthx8664::X64, ssa.reg), ctx, ssa.offset);
    }
    else{
        generate_registerx8664(mss, return_lowerreg_width(w, ssa.reg), ctx);
    }
    return mss;
}

bool move_ssa_to_ssax8664(MemoryStream& mss, contextx8664& ctx, func_ssa_context& dest, func_ssa_context& src, general_widthx8664 w, func_context& fctx){
    if(dest.mem && src.mem){
        ArenaReference<GeneralRegisterx86_64> alreg = get_temporary_register(mss, fctx, ctx);
        if(!alreg) return true;
        mss<<'\t';
        generate_movx8664(mss, w, ctx)<<' ';
        if(ctx.destlast){
            write_ssa_x8664(mss, ctx, src, w);
            mss<<", ";
            generate_registerx8664(mss, return_lowerreg_width(w, alreg), ctx);
        }
        else{
            generate_registerx8664(mss, return_lowerreg_width(w, alreg), ctx);
            mss<<", ";
            write_ssa_x8664(mss, ctx, src, w);
        }
        mss<<'\n'<<'\t';
        generate_movx8664(mss, w, ctx)<<' ';
        if(ctx.destlast){
            generate_registerx8664(mss, return_lowerreg_width(w, alreg), ctx);
            mss<<", ";
            write_ssa_x8664(mss, ctx, dest, w);
        }
        else{
            write_ssa_x8664(mss, ctx, dest, w);
            mss<<", ";
            generate_registerx8664(mss, return_lowerreg_width(w, alreg), ctx);
        }
        mss<<'\n';
    }
    else{
        mss<<'\t';
        generate_movx8664(mss, w, ctx)<<' ';
        if(ctx.destlast){
            write_ssa_x8664(mss, ctx, src, w);
            mss<<", ";
            write_ssa_x8664(mss, ctx, dest, w);
        }
        else{
            write_ssa_x8664(mss, ctx, dest, w);
            mss<<", ";
            write_ssa_x8664(mss, ctx, src, w);
        }
        mss<<'\n';
    }
    return false;
}

bool add_ssa_to_ssax8664(MemoryStream& mss, contextx8664& ctx, func_ssa_context& dest, func_ssa_context& src, general_widthx8664 w, func_context& fctx){
    if(dest.mem && src.mem){
        ArenaReference<GeneralRegisterx86_64> alreg = get_temporary_register(mss, fctx, ctx);
        if(!alreg) return true;
        mss<<'\t';
        generate_movx8664(mss, w, ctx)<<' ';
        if(ctx.destlast){
            write_ssa_x8664(mss, ctx, src, w);
            mss<<", ";
            generate_registerx8664(mss, return_lowerreg_width(w, alreg), ctx);
        }
        else{
            generate_registerx8664(mss, return_lowerreg_width(w, alreg), ctx);
            mss<<", ";
            write_ssa_x8664(mss, ctx, src, w);
        }
        mss<<'\n'<<'\t';
        generate_gmnemonicx8664("add", mss, w, ctx, true)<<' ';
        if(ctx.destlast){
            generate_registerx8664(mss, return_lowerreg_width(w, alreg), ctx);
            mss<<", ";
            write_ssa_x8664(mss, ctx, dest, w);
        }
        else{
            write_ssa_x8664(mss, ctx, dest, w);
            mss<<", ";
            generate_registerx8664(mss, return_lowerreg_width(w, alreg), ctx);
        }
        mss<<'\n';
    }
    else{
        mss<<'\t';
        generate_gmnemonicx8664("add", mss, w, ctx, true)<<' ';
        if(ctx.destlast){
            write_ssa_x8664(mss, ctx, src, w);
            mss<<", ";
            write_ssa_x8664(mss, ctx, dest, w);
        }
        else{
            write_ssa_x8664(mss, ctx, dest, w);
            mss<<", ";
            write_ssa_x8664(mss, ctx, src, w);
        }
        mss<<'\n';
    }
    return false;
}

bool handle_node_binaryop(MemoryStream& mss, func_context& fctx, contextx8664& ctx, ArenaReference<IRNode> node, func_ssa_context& alloc){
    switch(node->node_type){
        case IRNodeType::None:
            return true;
        case IRNodeType::Literal:{
                LiteralNode* lit = (LiteralNode*)node.get();
                general_widthx8664 w = general_widthx8664::X32;

                if((uintmax_t)lit->value >= UINT32_MAX){
                    w = general_widthx8664::X64;
                }
                
                if(w == general_widthx8664::X64 && alloc.mem == true){
                    ArenaReference<GeneralRegisterx86_64> alreg = get_temporary_register(mss, fctx, ctx);
                    if(!alreg) return true;
                    mss<<'\t';
                    generate_movx8664(mss, w, ctx)<<' ';
                    if(ctx.destlast){
                        generate_immediatex8664(mss, lit->value, ctx);
                        mss<<", ";
                        generate_registerx8664(mss, return_lowerreg_width(w, alreg), ctx);
                    }
                    else{
                        generate_registerx8664(mss, return_lowerreg_width(w, alreg), ctx);
                        mss<<", ";
                        generate_immediatex8664(mss, lit->value, ctx);
                    }
                    mss<<'\n'<<'\t';
                    generate_movx8664(mss, w, ctx)<<' ';
                    if(ctx.destlast){
                        generate_registerx8664(mss, return_lowerreg_width(w, alreg), ctx);
                        mss<<", ";
                        write_ssa_x8664(mss, ctx, alloc, w);
                    }
                    else{
                        write_ssa_x8664(mss, ctx, alloc, w);
                        mss<<", ";
                        generate_registerx8664(mss, return_lowerreg_width(w, alreg), ctx);
                    }
                    mss<<'\n';
                }
                else{
                    mss<<'\t';
                    generate_movx8664(mss, w, ctx)<<' ';
                    if(ctx.destlast){
                        generate_immediatex8664(mss, lit->value, ctx);
                        mss<<", ";
                        generate_registerx8664(mss, return_lowerreg_width(w, alloc.reg), ctx);
                    }
                    else{
                        generate_registerx8664(mss, return_lowerreg_width(w, alloc.reg), ctx);
                        mss<<", ";
                        generate_immediatex8664(mss, lit->value, ctx);
                    }
                    mss<<'\n';
                }
            }
            break;
        case IRNodeType::Argument:{
                func_ssa_context* argssa = find_ssa_x8664(fctx, node);
                if(!argssa) return true;
                if(move_ssa_to_ssax8664(mss, ctx, alloc, *argssa, general_widthx8664::X64, fctx)){
                    return true;
                }
            }
            break;
        case IRNodeType::Instruction:
            func_ssa_context* ssaf = find_ssa_x8664(fctx, node);
            if(!ssaf){
                ssaf = generate_instructionx8664(mss, fctx, ctx, node.cast<InstructionNode>());
                if(!ssaf) return true;
            }
            if(move_ssa_to_ssax8664(mss, ctx, alloc, *ssaf, general_widthx8664::X64, fctx)){
                return true;
            }
            break;
    }
    return false;
}

bool handle_node_binaryop_op(MemoryStream& mss, func_context& fctx, contextx8664& ctx, ArenaReference<IRNode> node, func_ssa_context& alloc, BinaryOPNode::BinaryOPType type){
    switch(node->node_type){
        case IRNodeType::None:
            return true;
        case IRNodeType::Literal:{
                LiteralNode* lit = (LiteralNode*)node.get();
                general_widthx8664 w = general_widthx8664::X32;

                if((uintmax_t)lit->value >= UINT32_MAX){
                    w = general_widthx8664::X64;
                }
                if(w == general_widthx8664::X64){
                    ArenaReference<GeneralRegisterx86_64> alreg = get_temporary_register(mss, fctx, ctx);
                    if(!alreg) return true;
                    mss<<'\t';
                    switch(type){
                        case BinaryOPNode::ADD:
                            generate_movx8664(mss, w, ctx)<<' ';
                            if(ctx.destlast){
                                generate_immediatex8664(mss, lit->value, ctx);
                                mss<<", ";
                                generate_registerx8664(mss, return_lowerreg_width(w, alreg), ctx);
                            }
                            else{
                                generate_registerx8664(mss, return_lowerreg_width(w, alreg), ctx);
                                mss<<", ";
                                generate_immediatex8664(mss, lit->value, ctx);
                            }
                            mss<<'\n'<<'\t';
                            generate_gmnemonicx8664("add", mss, w, ctx, true)<<' ';
                            if(ctx.destlast){
                                generate_registerx8664(mss, return_lowerreg_width(w, alreg), ctx);
                                mss<<", ";
                                write_ssa_x8664(mss, ctx, alloc, w);
                            }
                            else{
                                write_ssa_x8664(mss, ctx, alloc, w);
                                mss<<", ";
                                generate_registerx8664(mss, return_lowerreg_width(w, alreg), ctx);
                            }
                            break;
                    }
                    mss<<'\n';
                    
                }
                else{
                    mss<<'\t';
                    switch(type){
                        case BinaryOPNode::ADD:
                            generate_gmnemonicx8664("add", mss, w, ctx, true)<<' ';
                            if(ctx.destlast){
                                generate_immediatex8664(mss, lit->value, ctx);
                                mss<<", ";
                                write_ssa_x8664(mss, ctx, alloc, w);
                            }
                            else{
                                write_ssa_x8664(mss, ctx, alloc, w);
                                mss<<", ";
                                generate_immediatex8664(mss, lit->value, ctx);
                            }
                            break;
                    }
                    mss<<'\n';
                }
            }
            break;
        case IRNodeType::Argument:{
                func_ssa_context* argssa = find_ssa_x8664(fctx, node);
                if(!argssa) return true;
                if(add_ssa_to_ssax8664(mss, ctx, alloc, *argssa, general_widthx8664::X64, fctx)){
                    return true;
                }
            }
            break;
        case IRNodeType::Instruction:
            func_ssa_context* ssaf = find_ssa_x8664(fctx, node);
            if(!ssaf){
                ssaf = generate_instructionx8664(mss, fctx, ctx, node.cast<InstructionNode>());
                if(!ssaf) return true;
            }
            if(add_ssa_to_ssax8664(mss, ctx, alloc, *ssaf, general_widthx8664::X64, fctx)){
                return true;
            }
            break;
    }
    return false;
}

func_ssa_context* generate_binaryopx8664(MemoryStream& mss, func_context& fctx, contextx8664& ctx, ArenaReference<BinaryOPNode> node){
    func_ssa_context alloc = allocate_ssa_x8664(mss, fctx, ctx, node.cast<IRNode>());
    size_t inx = fctx.ssa_ctx.size();
    fctx.ssa_ctx.push_back(alloc);

    if(handle_node_binaryop(mss, fctx, ctx, node->lhs, alloc)) return nullptr;
    if(handle_node_binaryop_op(mss, fctx, ctx, node->rhs, alloc, node->optype)) return nullptr;

    return &fctx.ssa_ctx[inx];
}

func_ssa_context* generate_instructionx8664(MemoryStream& mss, func_context& fctx, contextx8664& ctx, ArenaReference<InstructionNode> node){
    switch(node->ins_type){
        case InstructionType::BINARYOP:
            return generate_binaryopx8664(mss, fctx, ctx, node.cast<BinaryOPNode>());
        case InstructionType::RET:
            return generate_returninstructionx8664(mss, fctx, ctx, node.cast<ReturnNode>());
        default:
            return nullptr;
    }
}

bool address_args(func_ssa_context& fctx, size_t argc, contextx8664& ctx, func_context& func){
    if(argc < ctx.tg->abi.reg_args.size()){
        fctx.reg = ctx.tg->abi.reg_args[argc];
    }
    else{
        fctx.mem = true;
        if(ctx.tg->useFramePointer){
            fctx.reg = ctx.tg->abi.framePointer;
        }
        else{
            fctx.reg = ctx.tg->abi.stackPointer;
        }
        fctx.offset = (func.stack + (8 * (argc - ctx.tg->abi.reg_args.size())));
    }
    return false;
}

bool generate_function_debugx8664(MemoryStream& mss, const Function& func, contextx8664& ctx){
    if(ctx.tg->syntax == TargetInfo::GNUAS){
        mss<<"\t.type "<<func.name<<", @function"<<'\n';
    }
    return false;
}

bool generate_end_function_debugx8664(MemoryStream& mss, const Function& func, contextx8664& ctx){
    if(ctx.tg->syntax == TargetInfo::GNUAS){
        mss<<".L"<<func.name<<"_end"<<':'<<'\n';
        mss<<"\t.size "<<func.name<<", ";
        mss<<".L"<<func.name<<"_end"<<'-'<<func.name<<'\n';
    }
    return false;
}

bool generate_functionx8664(MemoryStream& mss, const Function& func, contextx8664& ctx){
    mss<<'\t';
    generate_globalx8664(mss, ctx)<<' '<<func.name<<'\n'<<'\t';

    generate_alignx8664(mss, ctx.tg->abi.alignFunctions, ctx);
    mss<<'\n';

    if(generate_function_debugx8664(mss, func, ctx)){
        return true;
    }

    mss<<func.name<<':'<<'\n';
    func_context fctx;

    fctx.stack += ctx.tg->abi.shadowSpace;

    generate_prologuex8664(mss, ctx, fctx);
    
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
            if(!    generate_instructionx8664(mss, fctx, ctx, node.cast<InstructionNode>())){
                std::cout<<"Error codegen instruction"<<std::endl;
                return true;
            }
        }
        else{
            return true;
        }
    }

    if(function_ep_stackallocx8664(mss, ctx, fctx)){
        return true;
    }

    if(generate_end_function_debugx8664(mss, func, ctx)){
        return true;
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

bool generate_file_namex8664(MemoryStream& mss, const Frame& frame, contextx8664& ctx){
    if(!frame.file) return true;

    if(ctx.tg->syntax == TargetInfo::GNUAS){
        mss<<"\t.file "<<'"'<<frame.file->get_path().filename()<<'"'<<'\n';
    }

    return false;
}

bool Targetx86_64::generate_assembly(MemoryStream& mss, const Frame& frame, TargetInfo* info){
    if(!mss || !info || info->target != x86_64) return true;
    contextx8664 ctx;
    ctx.tg = (Targetx86_64*)info;
    ctx.destlast = (ctx.tg->syntax == GNUAS);
    if(!ctx.tg->regs) return true;

    if(generate_file_namex8664(mss, frame, ctx)){
        return true;
    }

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