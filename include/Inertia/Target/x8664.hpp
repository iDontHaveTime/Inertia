#ifndef INERTIA_X8664_HPP
#define INERTIA_X8664_HPP

#include "Inertia/IR/Frame.hpp"
#include "Inertia/Mem/Memstream.hpp"
#include "Inertia/Target/Target.hpp"

namespace Inertia{
    struct Registerx86_64{
        std::string_view name;
        int width; // in bits
        Registerx86_64* parent;
        Registerx86_64* child;
    };
    struct RegisterCollectionx86_64{

    };
    struct ABIx86_64{
        
    };
    struct Targetx86_64 : public TargetInfo{
        RegisterCollectionx86_64 regs;
        ABIx86_64 abi;

        Targetx86_64() noexcept : TargetInfo(x86_64){};

        bool generate_assembly(MemoryStream& mss, const Frame& frame, TargetInfo* info);
    };
}

#endif // INERTIA_X8664_HPP
