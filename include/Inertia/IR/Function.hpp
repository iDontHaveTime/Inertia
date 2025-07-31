#ifndef INERTIA_FUNCTION_HPP
#define INERTIA_FUNCTION_HPP

#include "Inertia/IR/Instruction.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Mem/Arenalloc.hpp"

namespace Inertia{
    struct Argument{
        std::string_view name;
        ArenaReference<Type> type;
    };
    class Function{
    public:
        std::string_view name;
        ArenaReference<Type> return_type;
        std::vector<Argument> args;
        std::vector<ArenaReference<Instruction>> instructions;

        Function() = default;

        ~Function() = default;

        Function(const Function&) = default;
        Function& operator=(const Function&) = default;

        Function(Function&& rhs){
            name = std::move(rhs.name);
            args = std::move(rhs.args);
            instructions = std::move(rhs.instructions);
            return_type = rhs.return_type;
            rhs.return_type.unreference();
        }

        Function& operator=(Function&& rhs){
            if(this != &rhs){
                name = std::move(rhs.name);
                args = std::move(rhs.args);
                instructions = std::move(rhs.instructions);
                return_type = rhs.return_type;
                rhs.return_type.unreference();
            }
            return *this;
        }

    };
}

#endif // INERTIA_FUNCTION_HPP
