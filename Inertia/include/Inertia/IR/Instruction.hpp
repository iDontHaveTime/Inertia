#ifndef INERTIA_INSTRUCTION_HPP
#define INERTIA_INSTRUCTION_HPP

#include "Inertia/Definition/Defines.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include <cstddef>
#include <cstdint>

/* This header contains everything related to the IR instructions. */

// This macro sets the TERMINATE flag of the IR instruction type. 
// This means this is the last instruction that the block can take.
#define TERM 0x8000

namespace Inertia{
    /* All the instructions that the IR has. */
    enum class IROpType : uint16_t{
        Unknown = 0, // Unknown Instruction.
        Add = 1, // Addition.
        Sub = 2, // Subtraction.
        Mul = 3, // Multiplication.
        Div = 4, // Unsigned Division.
        SDiv = 5, // Signed Division.
        Load = 6, // Load Mem.
        Store = 7, // Store mem.
        Ret = 8 | TERM, // Return instruction, which terminates the block too.
        Alloc = 9 // Allocate stack space.
    };
    /* The type of the SSA value provided. */
    enum class SSAType : uint16_t{
        NORMAL, // This is the default SSA.
        CONSTANT, // Constant SSA, like just a number.
        ARGUMENT // Function argument SSA.
    };

    /* The base class for all SSA types. */
    struct SSAValue{
        /* The ID of the SSA Value. */
        size_t id;
        /* Type of the SSA Value. */
        ArenaReference<Type> type;
        /* The type of SSA that the SSA Value is. */
        SSAType ssa_type;

        /* The default constructor. */
        SSAValue() noexcept = default;
        /* Constructor with the type parameter. */
        SSAValue(SSAType _ssa_type) noexcept : ssa_type(_ssa_type){};
        /* The correct constructor for a valid SSA Value. */
        SSAValue(size_t _id, const ArenaReference<Type>& _type, SSAType _ssa_type) noexcept : id(_id), type(_type), ssa_type(_ssa_type){};
    };

    /* The Function Argument SSA. */
    struct SSAArg : public SSAValue{

        /* Default constructor. */
        SSAArg() noexcept : SSAValue(SSAType::ARGUMENT){};
        /* The correct constructor for a valid SSA arg. */
        SSAArg(size_t _id, const ArenaReference<Type>& _type) noexcept : SSAValue(_id, _type, SSAType::ARGUMENT){};
    };

    /* The Constant Value SSA. */
    struct SSAConst : public SSAValue{
        inrint value; // TODO: Change the inrint to a pointer, so it can either hold extint or other values.

        SSAConst() noexcept : SSAValue(SSAType::CONSTANT){};
        /* The correct constructor for a valid SSA const. */
        SSAConst(size_t _id, const ArenaReference<Type>& _type, inrint _value) noexcept : SSAValue(_id, _type, SSAType::CONSTANT), value(_value){};
    };

    // Forward declare the Block class.
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
