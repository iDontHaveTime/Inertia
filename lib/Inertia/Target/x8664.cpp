#include "Inertia/Target/x8664.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include "Inertia/Mem/Memstream.hpp"
#include "Inertia/Target/Target.hpp"

namespace Inertia{

enum class general_widthx8664{
    X8, X16, X32, X64
};

struct saved_regx8664{
    ArenaReference<LowerRegisterx86_64> reg;
    general_widthx8664 w;
    saved_regx8664(ArenaReference<LowerRegisterx86_64>& r, general_widthx8664 wi) noexcept : reg(r), w(wi){};
};

struct contextx8664{
    Targetx86_64* tg;
    std::vector<saved_regx8664> saved;
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

bool save_registerx8664(MemoryStream& mss, ArenaReference<LowerRegisterx86_64> reg, contextx8664& ctx, general_widthx8664 w) noexcept{
    generate_pushx8664(mss, w, ctx)<<' ';   
    generate_registerx8664(mss, reg, ctx);
    ctx.saved.emplace_back(reg, w);
    return false;
}

bool unsave_registerx8664(MemoryStream& mss, ArenaReference<LowerRegisterx86_64> reg, contextx8664& ctx, general_widthx8664 w) noexcept{
    generate_popx8664(mss, w, ctx)<<' ';
    generate_registerx8664(mss, reg, ctx);
    return false;
}

bool unsave_all_registersx8664(MemoryStream& mss, contextx8664& ctx) noexcept{
    for(auto it = ctx.saved.rbegin(); it != ctx.saved.rend(); it++){
        mss<<'\t';
        unsave_registerx8664(mss, it->reg, ctx, it->w);
        mss<<'\n';
    }
    return false;
}

bool generate_prologuex8664(MemoryStream& mss, contextx8664& ctx) noexcept{

    if(ctx.tg->abi.framePointer->flags & RegisterFlags::CALEE_SAVED){
        mss<<'\t';
        save_registerx8664(mss, ctx.tg->abi.framePointer->l64b, ctx, general_widthx8664::X64);
        mss<<'\n';
    }


    return false;
}

bool generate_epiloguex8664(MemoryStream& mss, contextx8664& ctx) noexcept{

    if(ctx.tg->abi.framePointer->flags & RegisterFlags::CALEE_SAVED){
        unsave_all_registersx8664(mss, ctx);
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

bool generate_functionx8664(MemoryStream& mss, const Function& func, contextx8664& ctx){
    mss<<'\t';
    generate_globalx8664(mss, ctx)<<' '<<func.name<<'\n'<<'\t';

    generate_alignx8664(mss, ctx.tg->abi.alignFunctions, ctx);
    mss<<'\n';

    mss<<func.name<<':'<<'\n';

    if(ctx.tg->useFramePointer && ctx.tg->abi.framePointer){
        if(generate_prologuex8664(mss, ctx)){
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
    if(!ctx.tg->regs) return true;

    // md

    if(generate_sectionx8664(mss, ctx.tg->execSection, ctx)) return true;
    for(const Function& func : frame.funcs){
        generate_functionx8664(mss, func, ctx);
    }

    return false;
}

}