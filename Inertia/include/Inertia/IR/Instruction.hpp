#ifndef INERTIA_INSTRUCTION_HPP
#define INERTIA_INSTRUCTION_HPP

#include "Inertia/Definition/Defines.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include <cstddef>
#include <cstdint>

#define TERM 0x8000

namespace Inertia{
    enum class IROpType : uint16_t{
        Unknown = 0,
        Add = 1,
        Sub = 2,
        Mul = 3,
        Div = 4,
        Load = 5,
        Store = 6,
        Ret = 7 | TERM,
        Alloc = 8
    };
    enum class SSAType : uint16_t{
        NORMAL, CONSTANT, ARGUMENT
    };

    struct SSAValue{
        size_t id;
        ArenaReference<Type> type;
        SSAType ssa_type;

        SSAValue() noexcept = default;
        SSAValue(SSAType _ssa_type) noexcept : ssa_type(_ssa_type){};
        SSAValue(size_t _id, const ArenaReference<Type>& _type, SSAType _ssa_type) noexcept : id(_id), type(_type), ssa_type(_ssa_type){};
    };

    struct SSAArg : public SSAValue{

        SSAArg() noexcept : SSAValue(SSAType::ARGUMENT){};
        SSAArg(size_t _id, const ArenaReference<Type>& _type) noexcept : SSAValue(_id, _type, SSAType::ARGUMENT){};
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

    struct SSACtx{
        size_t ends;
        bool force;
    };
}

#endif // INERTIA_INSTRUCTION_HPP
