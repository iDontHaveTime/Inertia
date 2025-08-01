#include "Inertia/Target/x8664.hpp"

namespace Inertia{

bool Targetx86_64::generate_assembly(MemoryStream& mss, const Frame& frame, TargetInfo* info){
    if(!mss || !info) return true;
    Targetx86_64* xinfo = (Targetx86_64*)info;
    if(!xinfo->regs) return true;

    

    return false;
}

}