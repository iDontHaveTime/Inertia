#ifndef INERTIA_INSTRUCTION_HPP
#define INERTIA_INSTRUCTION_HPP

#include "Inertia/IR/IRNode.hpp"
#include "Inertia/Mem/Arenalloc.hpp"

namespace Inertia{
    enum class InstructionType{
        RET,
    };
    class InstructionNode : public IRNode{
    public:
        InstructionType ins_type;

        InstructionNode() noexcept : IRNode(IRNodeType::Instruction){};
        InstructionNode(InstructionType it) noexcept : IRNode(IRNodeType::Instruction), ins_type(it){};
    };
    class ReturnNode : public InstructionNode{
        ReturnNode() noexcept : InstructionNode(InstructionType::RET){};
        ArenaReference<IRNode> node;
    };
}

#endif // INERTIA_INSTRUCTION_HPP
