#ifndef INERTIA_INSTRSELX86_HPP
#define INERTIA_INSTRSELX86_HPP

#include "Inertia/Lowering/Generic/InstrSelGen.hpp"

namespace Inertia{
    class InstructionSelectorx86 : public InstructionSelectorGeneric{
    public:

        bool lower(const Frame& frame, LoweredOutput& to) override;
    };
}

#endif // INERTIA_INSTRSELX86_HPP
