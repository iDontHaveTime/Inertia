#include "Inertia/Target/TargetBase.hpp"

namespace InertiaTarget{
void TargetBase::init(){
    
}
TargetBase::~TargetBase() noexcept = default;

void TargetInstruction::emit(std::ostream& os){
    os<<"(null)";
}
}