#ifndef INERTIA_INSTRUCTION_HPP
#define INERTIA_INSTRUCTION_HPP

#include "Inertia/IR/Type.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include <cstddef>
#include <cstdint>

#define __makebitwiseflags(type, name) \
    inline bool check_flag(type __flag__) const noexcept{ \
        return name & __flag__; \
    } \
    inline void set_flag(type __flag__) noexcept{ \
        name |= __flag__; \
    } \
    inline void remove_flag(type __flag__) noexcept{ \
        name &= ~__flag__; \
    }

namespace Inertia{
    enum class IROpType : uint16_t{
        Unknown,
        Add,
        Sub,
        Mul,
        Div,
        Mov,
        Load,
        Store,
        Ret
    };

    struct SSAValue{
        size_t id;
        ArenaReference<Type> type;

        SSAValue() noexcept = default;
        SSAValue(size_t _id, ArenaReference<Type> _type) noexcept : id(_id), type(_type){};
    };

    struct IRInstruction{
        ArenaAlloc* arena;
        struct Block* parent;
        IROpType op;

        IRInstruction(Block* _parent, ArenaAlloc* _arena, IROpType _op = IROpType::Unknown) noexcept : arena(_arena), parent(_parent), op(_op){};
    };
}

#undef __makebitwiseflags

#endif // INERTIA_INSTRUCTION_HPP
