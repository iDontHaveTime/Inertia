#ifndef INERTIA_INSTRSELMAN_HPP
#define INERTIA_INSTRSELMAN_HPP

#include "Inertia/Lowering/Generic/InstrSelGen.hpp"
#include "Inertia/Target/Triple.hpp"

namespace Inertia{
    class InstructionSelectorManager{
        InstructionSelectorGeneric* internal = nullptr;
        TargetType tt = TargetType::None;
    public:

        InstructionSelectorManager() noexcept = default;

        InstructionSelectorManager(TargetType _tt) noexcept : tt(_tt){
            load_target(_tt);
        }

        InstructionSelectorManager(const TargetTriple& _tt) noexcept : tt(_tt.getLoadedType()){
            load_target(_tt.getLoadedType());
        }

        void load_target(TargetType _tt);

        inline void load_target(const TargetTriple& _tt){
            load_target(_tt.getLoadedType());
        }

        inline void close_target() noexcept{
            if(internal){
                delete internal;
                internal = nullptr;
            }
            tt = TargetType::None;
        }

        inline TargetType getLoadedType() const noexcept{
            return tt;
        }

        inline bool lower(Frame& frame, LoweredOutput& to){
            if(!internal) return true;
            return internal->lower(frame, to);
        }

        inline InstructionSelectorGeneric* getRaw() noexcept{
            return internal;
        }

        ~InstructionSelectorManager() noexcept{
            close_target();
        }
    };
}

#endif // INERTIA_INSTRSELMAN_HPP
