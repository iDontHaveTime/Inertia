// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_INSTRUCTION_H
#define INERTIA_IR_INSTRUCTION_H

/// @file IR/Instruction.h
/// @brief Contains the instruction class.

#include <inr/ADT/IList.h>
#include <inr/ADT/IVector.h>
#include <inr/IR/Value.h>

namespace inr {

/// @brief The base instruction class for all instructions.
class Instruction : public Value, public ilist_node<Instruction> {
public:
    enum class InstructionID { RETURN, ADD };

    static sview getInstructionIDStr(InstructionID);

protected:
    /// @brief The block that the instruction is in.
    class Block* parent_;
    /// @brief The instruction's operands.
    ivec<Value*, 3> operands_;

    /// @brief What instruction this is.
    InstructionID insID_;

    /// @brief This appends the instruction to the block given.
    /// @param to The block the instruction should go to.
    /// @param ins The instruction to insert.
    static void append(Block* to, Instruction* ins);

    /// @brief The basic constructor of an instruction.
    /// @param id What instruction it is.
    /// @param type The type of the instruction.
    /// @param parent The block that this instruction is in.
    /// @param operands The operands of the instruction.
    /// @param name The name of this instruction.
    Instruction(InstructionID id, const Type* type, Block* parent,
                ivec<Value*, 3> operands, sview name = "") :
        Value(ValueID::Instruction, type, name),
        parent_(parent),
        operands_(std::move(operands)),
        insID_(id) {
        append(parent, this);
        for(Value* op : operands_) {
            op->addUser(this);
        }
    }

    ~Instruction() noexcept override {
        for(Value* op : operands_) op->removeUser(this);
    }

public:
    /// @brief Gets the instruction type.
    InstructionID getID() const noexcept {
        return insID_;
    }

    /// @brief Gets the parent of the instruction.
    /// @return Const pointer to the block.
    const Block* getParent() const noexcept {
        return parent_;
    }

    /// @brief Gets the parent of the instruction.
    /// @return Pointer to the block.
    Block* getParent() noexcept {
        return parent_;
    }

    /// @brief Gets the operands of the instruction.
    /// @return Const reference to the vector.
    const ivec<Value*, 3>& getOperands() const noexcept {
        return operands_;
    }

    /// @brief Gets the operand at a certain index.
    /// @return Pointer to the value.
    Value* getOperand(unsigned i) const {
        return operands_.at(i);
    }

    /// @brief Gets the number of operands the instruction has.
    unsigned getNumOperands() const noexcept {
        return operands_.size();
    }

    virtual bool isTerminator() const noexcept {
        return false;
    }

    /// @brief Returns if the instruction can be safely removed.
    static bool removeable(const Instruction& inst) {
        return inst.isVolatile() ? false
                                 : (inst.getUsers().empty()
                                        ? (inst.isTerminator() ? false : true)
                                        : false);
    }

    friend class Block;
};

/// @brief Instruction with the terminator function overriden.
class TermInst : public Instruction {
public:
    TermInst(InstructionID id, const Type* type, Block* parent,
             ivec<Value*, 3> operands, sview name = "") :
        Instruction(id, type, parent, operands, name) {}

    bool isTerminator() const noexcept override {
        return true;
    }
};

/// @brief The "ret" (Return) instruction.
class ReturnInst : public TermInst {
    /// @brief The basic return instruction constructor.
    /// @see create(Value*, Block*) to get more details.
    ReturnInst(Value* retVal, Block* parent);

public:
    /// @brief Constructs a new return instruction to the block.
    /// @param retVal The value of this return, can be set to nullptr for void.
    /// @param parent The block that this instruction will go to.
    /// @return The instruction.
    static ReturnInst* create(Value* retVal, Block* parent) {
        return new ReturnInst(retVal, parent);
    }
};

class BinaryInst : public Instruction {
    /// @brief The basic binary op instruction constructor.
    BinaryInst(InstructionID id, Value* lhs, Value* rhs, sview name,
               Block* parent) :
        Instruction(id, lhs->getType(), parent, {lhs, rhs}, name) {}

public:
    /// @brief Creates a new add instruction.
    /// @param lhs The left hand side of the addition.
    /// @param rhs The right hand side of the addition.
    /// @param name The name of the result.
    /// @param parent The block that this instruction goes to.
    /// @return The instruction.
    static BinaryInst* createAdd(Value* lhs, Value* rhs, sview name,
                                 Block* parent) {
        return new BinaryInst(InstructionID::ADD, lhs, rhs, name, parent);
    }
};

/// @brief Outputs the instruction to the stream.
/// @note Does not insert a semicolon.
raw_stream& operator<<(raw_stream&, const Instruction&);

} // namespace inr

#endif // INERTIA_IR_INSTRUCTION_H
