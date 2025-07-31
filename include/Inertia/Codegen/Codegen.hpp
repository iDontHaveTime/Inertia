#ifndef INERTIA_CODEGEN_HPP
#define INERTIA_CODEGEN_HPP

#include "Inertia/IR/Frame.hpp"
#include "Inertia/Mem/Memstream.hpp"
#include "Inertia/Target/Target.hpp"

namespace Inertia{
    class IRCodegen{
    public:
        
        bool codegen_assembly(MemoryStream& mems, const Frame& parsed, TargetInfo* info);
    };
}

#endif // INERTIA_CODEGEN_HPP
