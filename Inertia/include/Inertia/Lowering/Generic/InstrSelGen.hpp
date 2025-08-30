#ifndef INERTIA_INSTRSELGEN_HPP
#define INERTIA_INSTRSELGEN_HPP

#include "Inertia/IR/Frame.hpp"
#include "Inertia/Lowering/LoweredOut.hpp"

namespace Inertia{
    class InstructionSelectorGeneric{
    protected:
        inline void reserve_lower(const Frame& frame, LoweredOutput& to){
            to.funcs.reserve(frame.funcs.size());
        }

    public:

        InstructionSelectorGeneric() noexcept = default;

        virtual bool lower(Frame& frame, LoweredOutput& to, InertiaTarget::TargetBase* tb);
        virtual ~InstructionSelectorGeneric() noexcept = default;
    };
}

#endif // INERTIA_INSTRSELGEN_HPP
