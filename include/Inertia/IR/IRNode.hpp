#ifndef INERTIA_IRNODE_HPP
#define INERTIA_IRNODE_HPP

#include "Inertia/IR/Type.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include <cstdint>
#include <string_view>

namespace Inertia{
    enum class IRNodeType{
        None,
        Literal,
        Argument,
        Instruction,
    };
    class IRNode{
    public:
        std::string_view name;
        ArenaReference<Type> type;
        IRNodeType node_type = IRNodeType::None;

        IRNode() = default;
        IRNode(IRNodeType t) noexcept : node_type(t){};
    };
    class LiteralNode : public IRNode{
    public:
        intmax_t value;
        LiteralNode() : IRNode(IRNodeType::Literal), value(0){};
    };
}

#endif // INERTIA_IRNODE_HPP
