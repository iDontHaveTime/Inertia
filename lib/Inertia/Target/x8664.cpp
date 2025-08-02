#include "Inertia/Target/x8664.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include "Inertia/Mem/Memstream.hpp"
#include "Inertia/Target/Target.hpp"

namespace Inertia{

MemoryStream& generate_registerx8664(MemoryStream& mss, ArenaReference<LowerRegisterx86_64> reg, Targetx86_64* tg) noexcept{
    switch(tg->syntax){
        case TargetInfo::GNUAS:
            mss<<'%'<<reg->name;
            break;
        default:
            mss<<reg->name;
            break;
    }
    return mss;
}

enum class general_widthx8664{
    X8, X16, X32, X64
};

MemoryStream& generate_widthx8664(MemoryStream& mss, general_widthx8664 w, Targetx86_64* tg) noexcept{
    switch(tg->syntax){
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

MemoryStream& generate_pushx8664(MemoryStream& mss, general_widthx8664 w, Targetx86_64* tg) noexcept{
    switch(tg->syntax){
        case TargetInfo::GNUAS:
            mss<<"push";
            generate_widthx8664(mss, w, tg);
            break;
        default:
            mss<<"push";
            break;
    }
    return mss;
}

MemoryStream& generate_popx8664(MemoryStream& mss, general_widthx8664 w, Targetx86_64* tg) noexcept{
    switch(tg->syntax){
        case TargetInfo::GNUAS:
            mss<<"pop";
            generate_widthx8664(mss, w, tg);
            break;
        default:
            mss<<"pop";
            break;
    }
    return mss;
}

MemoryStream& generate_retx8664(MemoryStream& mss, Targetx86_64* tg) noexcept{
    switch(tg->syntax){
        case TargetInfo::GNUAS:
            mss<<"ret";
            break;
        default:
            mss<<"ret";
            break;
    }
    return mss;
}

bool save_registerx8664(MemoryStream& mss, ArenaReference<LowerRegisterx86_64> reg, Targetx86_64* tg, general_widthx8664 w) noexcept{
    generate_pushx8664(mss, w, tg)<<' ';
    generate_registerx8664(mss, reg, tg);
    return false;
}

bool unsave_registerx8664(MemoryStream& mss, ArenaReference<LowerRegisterx86_64> reg, Targetx86_64* tg, general_widthx8664 w) noexcept{
    generate_popx8664(mss, w, tg)<<' ';
    generate_registerx8664(mss, reg, tg);
    return false;
}

bool generate_prologuex8664(MemoryStream& mss, Targetx86_64* tg) noexcept{

    if(tg->abi.framePointer->flags & RegisterFlags::CALEE_SAVED){
        mss<<'\t';
        save_registerx8664(mss, tg->abi.framePointer->l64b, tg, general_widthx8664::X64);
        mss<<'\n';
    }


    return false;
}

bool generate_epiloguex8664(MemoryStream& mss, Targetx86_64* tg) noexcept{

    if(tg->abi.framePointer->flags & RegisterFlags::CALEE_SAVED){
        mss<<'\t';
        unsave_registerx8664(mss, tg->abi.framePointer->l64b, tg, general_widthx8664::X64);
        mss<<'\n';
    }


    return false;
}

MemoryStream& generate_globalx8664(MemoryStream& mss, TargetInfo* tg) noexcept{
    switch(tg->syntax){
        case TargetInfo::GNUAS:
            mss<<".globl";
            break;
        default:
            mss<<"global";
            break;
    }

    return mss;
}

void generate_alignx8664(MemoryStream& mss, int align, Targetx86_64* tg) noexcept{
    switch(tg->syntax){
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

bool generate_functionx8664(MemoryStream& mss, const Function& func, Targetx86_64* tg){
    mss<<'\t';
    generate_globalx8664(mss, tg)<<' '<<func.name<<'\n'<<'\t';

    generate_alignx8664(mss, tg->abi.alignFunctions, tg);
    mss<<'\n';

    mss<<func.name<<':'<<'\n';

    if(tg->useFramePointer && tg->abi.framePointer){
        if(generate_prologuex8664(mss, tg)){
            return true;
        }
    }

    return false;
}

bool generate_sectionx8664(MemoryStream& mss, const char* section, TargetInfo* tg) noexcept{
    switch(tg->syntax){
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
    Targetx86_64* xinfo = (Targetx86_64*)info;
    if(!xinfo->regs) return true;

    // md

    if(generate_sectionx8664(mss, xinfo->execSection, info)) return true;
    for(const Function& func : frame.funcs){
        generate_functionx8664(mss, func, xinfo);
    }

    return false;
}

}