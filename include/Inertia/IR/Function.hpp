#ifndef INERTIA_FUNCTION_HPP
#define INERTIA_FUNCTION_HPP

#include "Inertia/IR/IRNode.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Mem/Arenalist.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include <string_view>
#include <unordered_map>

namespace Inertia{
    struct Argument : public IRNode{
    public:
        Argument() noexcept : IRNode(IRNodeType::Argument){};
    };
    class Function{
    public:
        std::string_view name;
        std::vector<Argument> args;
        std::unordered_map<std::string_view, ArenaReference<IRNode>> ssa;
        ArenaLList<IRNode> instructions;

        ArenaReference<Type> retType;

    };
}

#endif // INERTIA_FUNCTION_HPP
