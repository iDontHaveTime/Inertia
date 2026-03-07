#ifndef INERTIA_IR_INSTRUCTION_H
#define INERTIA_IR_INSTRUCTION_H

/// @file IR/Instruction.h
/// @brief Contains the instruction class.

#include <inr/ADT/IList.h>
#include <inr/IR/Value.h>

#include <stdexcept>

namespace inr {

/// @brief The base instruction class for all instructions.
class Instruction : public Value, public ilist_node<Instruction> {
public:
    enum class InstructionID { RETURN, ADD };

protected:
    /// @brief The block that the instruction is in.
    class Block* parent_;
    /// @brief The instruction's operands.
    std::vector<Value*> operands_;

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
                std::vector<Value*> operands, sview name = "") :
        Value(ValueID::Instruction, type, name),
        parent_(parent),
        operands_(std::move(operands)),
        insID_(id) {
        append(parent, this);
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
    const std::vector<Value*>& getOperands() const noexcept {
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

    friend class Block;
};

/// @brief The "ret" (Return) instruction.
class ReturnInst : public Instruction {
    /// @brief The basic return instruction constructor.
    /// @see Create(Value*, Block*) to get more details.
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
        Instruction(id, lhs->getType(), parent, {lhs, rhs}, name) {
        if(lhs->getType() != rhs->getType())
            throw std::runtime_error(
                "Types do not match in a new binary op instruction.");
    }

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
