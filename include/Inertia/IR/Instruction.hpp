#ifndef INERTIA_INSTRUCTION_HPP
#define INERTIA_INSTRUCTION_HPP

#include "Inertia/IR/IRNode.hpp"
#include "Inertia/Mem/Arenalloc.hpp"

namespace Inertia{
    enum class InstructionType{
        RET,
        BINARYOP,
    };
    class InstructionNode : public IRNode{
    public:
        InstructionType ins_type;

        InstructionNode() noexcept : IRNode(IRNodeType::Instruction){};
        InstructionNode(InstructionType it) noexcept : IRNode(IRNodeType::Instruction), ins_type(it){};
    };
    class ReturnNode : public InstructionNode{
    public:
        ReturnNode() noexcept : InstructionNode(InstructionType::RET){};
        ArenaReference<IRNode> node;
    };
    class BinaryOPNode : public InstructionNode{
    public:
        BinaryOPNode() noexcept : InstructionNode(InstructionType::BINARYOP){};
        enum BinaryOPType {ADD} optype;
        ArenaReference<IRNode> lhs;
        ArenaReference<IRNode> rhs;
    };
}

#endif // INERTIA_INSTRUCTION_HPP
