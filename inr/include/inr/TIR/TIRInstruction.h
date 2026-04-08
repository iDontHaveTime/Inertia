// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TIRINSTRUCTION_H
#define INERTIA_TIR_TIRINSTRUCTION_H

/// @file TIR/TIRInstruction.h
/// @brief Contains the Target IR instruction class.

#include <inr/ADT/ArrView.h>
#include <inr/ADT/IList.h>
#include <inr/ADT/IVector.h>
#include <inr/IR/Constant.h>
#include <inr/IR/Type.h>
#include <inr/MIR/Register.h>

#include <variant>

namespace inr {

class TIRInstruction;

class TIROperand {
private:
    /// @brief Can contain either a register, or an integer.
    ///
    /// The integer represents a frame index.
    /// The constant integer represents an immediate.
    std::variant<Register, int64_t, const ConstantInt*> data_;

public:
    TIROperand() noexcept : data_((int64_t)0) {}

    TIROperand(Register reg) noexcept : data_(reg) {}
    TIROperand(int64_t mem) noexcept : data_(mem) {}
    TIROperand(const ConstantInt* imm) noexcept : data_(imm) {}

    TIROperand(const TIROperand&) noexcept = default;
    TIROperand& operator=(const TIROperand&) noexcept = default;
    TIROperand(TIROperand&&) noexcept = default;
    TIROperand& operator=(TIROperand&&) noexcept = default;

    bool isReg() const noexcept {
        return std::holds_alternative<Register>(data_);
    }

    bool isMem() const noexcept {
        return std::holds_alternative<int64_t>(data_);
    }

    bool isImm() const noexcept {
        return std::holds_alternative<const ConstantInt*>(data_);
    }

    Register getReg() const noexcept {
        return std::get<Register>(data_);
    }

    int64_t getMem() const noexcept {
        return std::get<int64_t>(data_);
    }

    const ConstantInt* getImm() const noexcept {
        return std::get<const ConstantInt*>(data_);
    }
};

/// @brief Possible TIR instructions.
enum class TIRInstID : unsigned {
    // Start
    TIR_INST_START,

    STORE,

    ADD_DEST_SRC,
    ADD_DEST_SRC_SRC,

    RET,

    // End
    TIR_INST_END
};

struct TIRInstInfo {
    bool enabled;

    constexpr TIRInstInfo() noexcept : enabled(true) {}

    constexpr TIRInstInfo(bool e) noexcept : enabled(e) {}
};

/// @brief Represents a TIR instruction.
/// @note This is the base class, and should not be used.
class TIRInstruction : public ilist_node<TIRInstruction> {
    class TIRBlock* parent_;
    const Type* type_;
    ivec<TIROperand*, 3> operands_;
    TIRInstID id_;

protected:
    /// @brief Constructs a new TIR instruction.
    TIRInstruction(TIRBlock* parent, const Type* type,
                   ivec<TIROperand*, 3> operands, TIRInstID id) noexcept :
        parent_(parent), type_(type), operands_(std::move(operands)), id_(id) {}

public:
    /// @brief Returns the instruction type.
    TIRInstID getInstID() const noexcept {
        return id_;
    }

    TIRBlock* getParent() noexcept {
        return parent_;
    }

    const TIRBlock* getParent() const noexcept {
        return parent_;
    }

    const Type* getType() const noexcept {
        return type_;
    }

    arrview<TIROperand*> getOperands() const noexcept {
        return operands_;
    }

    ivec<TIROperand*, 3>& getOperands() noexcept {
        return operands_;
    }

    const TIROperand* getOperand(unsigned n) const noexcept {
        return operands_[n];
    }

    TIROperand* getOperand(unsigned n) noexcept {
        return operands_[n];
    }

    void addOperand(TIROperand* operand) {
        operands_.emplace_back(operand);
    }

    unsigned getNumOperands() const noexcept {
        return operands_.size();
    }

    virtual ~TIRInstruction() noexcept = default;
};

class TIRStore : public TIRInstruction {
public:
    TIRStore(TIRBlock* parent, const Type* type, TIROperand* dest,
             TIROperand* src) :
        TIRInstruction(parent, type, {dest, src}, TIRInstID::STORE) {}
};

class TIRAdd : public TIRInstruction {
public:
    TIRAdd(TIRBlock* parent, const Type* type, TIROperand* dest,
           TIROperand* src) :
        TIRInstruction(parent, type, {dest, src}, TIRInstID::ADD_DEST_SRC) {}

    TIRAdd(TIRBlock* parent, const Type* type, TIROperand* dest,
           TIROperand* src1, TIROperand* src2) :
        TIRInstruction(parent, type, {dest, src1, src2},
                       TIRInstID::ADD_DEST_SRC_SRC) {}
};

class TIRRet : public TIRInstruction {
public:
    TIRRet(TIRBlock* parent, const Type* type) :
        TIRInstruction(parent, type, {}, TIRInstID::RET) {}
};

} // namespace inr

#endif // INERTIA_TIR_TIRINSTRUCTION_H
