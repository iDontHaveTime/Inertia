#ifndef INERTIA_INSTRUCTION_HPP
#define INERTIA_INSTRUCTION_HPP

#include "Inertia/Definition/Defines.hpp"
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
        Ret,
        Alloc
    };
    enum class SSAType : uint16_t{
        NORMAL, CONSTANT
    };

    struct SSAValue{
        size_t id;
        ArenaReference<Type> type;
        SSAType ssa_type;

        SSAValue() noexcept = default;
        SSAValue(SSAType _ssa_type) noexcept : ssa_type(_ssa_type){};
        SSAValue(size_t _id, const ArenaReference<Type>& _type, SSAType _ssa_type) noexcept : id(_id), type(_type), ssa_type(_ssa_type){};
    };

    struct SSAConst : public SSAValue{
        inrint value;

        SSAConst() noexcept : SSAValue(SSAType::CONSTANT){};
        SSAConst(size_t _id, const ArenaReference<Type>& _type, inrint _value) noexcept : SSAValue(_id, _type, SSAType::CONSTANT), value(_value){};
    };

    struct Block;

    struct IRInstruction{
        ArenaAlloc* arena;
        ArenaReference<Block> parent;
        IROpType op;

        IRInstruction(const ArenaReference<Block>& _parent, ArenaAlloc* _arena, IROpType _op = IROpType::Unknown) noexcept : arena(_arena), parent(_parent), op(_op){};
    };

    struct IRReturn : public IRInstruction{
        ArenaReference<SSAValue> src;

        IRReturn(const ArenaReference<SSAValue>& _src, const ArenaReference<Block>& _parent, ArenaAlloc* _arena) noexcept : IRInstruction(_parent, _arena, IROpType::Ret), src(_src){};
    };

    struct IRAlloc : public IRInstruction{
        ArenaReference<SSAValue> dest;
        size_t amount;

        IRAlloc(const ArenaReference<SSAValue>& _dest, size_t _amount, const ArenaReference<Block>& _parent, ArenaAlloc* _arena) noexcept : IRInstruction(_parent, _arena, IROpType::Alloc), dest(_dest), amount(_amount){};
    };

    struct IRBinaryOP : public IRInstruction{
        ArenaReference<SSAValue> dest;
        ArenaReference<SSAValue> lhs, rhs;

        IRBinaryOP(const ArenaReference<SSAValue>& _dest, const ArenaReference<SSAValue>& _lhs, const ArenaReference<SSAValue>& _rhs, const ArenaReference<Block>& _parent, ArenaAlloc* _arena, IROpType binoptype) noexcept : IRInstruction(_parent, _arena, binoptype), dest(_dest), lhs(_lhs), rhs(_rhs){};
    };
}

#undef __makebitwiseflags

#endif // INERTIA_INSTRUCTION_HPP
