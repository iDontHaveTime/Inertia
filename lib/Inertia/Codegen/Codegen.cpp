#include "Inertia/Codegen/Codegen.hpp"
#include "Inertia/Target/x8664.hpp"

namespace Inertia{

bool IRCodegen::codegen_assembly(MemoryStream& mems, const Frame& parsed, TargetInfo* info){
    switch(info->target){
        case TargetInfo::x86_64:
            return ((Targetx86_64*)info)->generate_assembly(mems, parsed, info);
        default:
            std::cout<<"Unknown target selected"<<std::endl;
            return true;
    }
}


}