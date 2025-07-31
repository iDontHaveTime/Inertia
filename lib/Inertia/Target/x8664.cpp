#include "Inertia/Target/x8664.hpp"

namespace Inertia{

bool Targetx86_64::generate_assembly(MemoryStream& mss, const Frame& frame, TargetInfo* info){
    if(!mss || !info) return true;
    mss<<"Hello world";
    return false;
}

}