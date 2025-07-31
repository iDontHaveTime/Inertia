#ifndef INERTIA_INSTRUCTION_HPP
#define INERTIA_INSTRUCTION_HPP

#include "Inertia/IR/Type.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include <string_view>

namespace Inertia{
    struct Instruction{
        enum InstructionType {RET} ins_type;

        Instruction() = default;
        Instruction(InstructionType t) noexcept : ins_type(t){};
    };

    struct ReturnInstruction : public Instruction{
        ArenaReference<Type> type;
        enum ReturnType {INTEGER, FLOAT, SSA} ret_type;
        std::string_view ret_val;
        bool negative;

        ReturnInstruction() noexcept : Instruction(RET){};
    };
}

#endif // INERTIA_INSTRUCTION_HPP
