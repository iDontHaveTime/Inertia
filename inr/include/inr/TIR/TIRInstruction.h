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
#include <inr/IR/Global.h>
#include <inr/IR/Type.h>
#include <inr/MIR/Register.h>

#include <initializer_list>
#include <variant>

namespace inr {

class TIRInstruction;

/// @brief Represents an instruction operand.
class TIROperand {
private:
    /// @brief Can contain either a register, or an integer.
    ///
    /// The integer represents a frame index.
    /// The constant integer represents an immediate.
    /// nullptr_t exists for "none" operands, mainly used for describing
    /// instructions.
    std::variant<Register, MemOperand, const ConstantInt*, const Global*,
                 std::monostate>
        data_;

public:
    TIROperand() noexcept : data_(std::monostate{}) {}

    TIROperand(Register reg) noexcept : data_(reg) {}
    TIROperand(MemOperand mem) noexcept : data_(mem) {}
    TIROperand(const ConstantInt* imm) noexcept : data_(imm) {}
    TIROperand(const Global* glob) noexcept : data_(glob) {}

    TIROperand(const TIROperand&) noexcept = default;
    TIROperand& operator=(const TIROperand&) noexcept = default;
    TIROperand(TIROperand&&) noexcept = default;
    TIROperand& operator=(TIROperand&&) noexcept = default;

    bool isReg() const noexcept {
        return std::holds_alternative<Register>(data_);
    }

    bool isMemReg() const noexcept {
        return std::holds_alternative<MemOperand>(data_);
    }

    bool isGlobal() const noexcept {
        return std::holds_alternative<const Global*>(data_);
    }

    bool isMem() const noexcept {
        return isMemReg() || isGlobal();
    }

    bool isImm() const noexcept {
        return std::holds_alternative<const ConstantInt*>(data_);
    }

    bool isNone() const noexcept {
        return std::holds_alternative<std::monostate>(data_);
    }

    Register getReg() const noexcept {
        return std::get<Register>(data_);
    }

    MemOperand getMemReg() const noexcept {
        return std::get<MemOperand>(data_);
    }

    const Global* getGlobal() const noexcept {
        return std::get<const Global*>(data_);
    }

    const ConstantInt* getImm() const noexcept {
        return std::get<const ConstantInt*>(data_);
    }

    bool operator==(const TIROperand& other) const noexcept = default;
};

/// @brief Possible TIR instructions.
enum class TIRInstID : unsigned {
    // Start
    TIR_INST_START,

    STORE,

    ADD_DEST_SRC,
    ADD_DEST_SRC_SRC,

    SUB_DEST_SRC,
    SUB_DEST_SRC_SRC,

    RET,

    // End
    TIR_INST_END
};

/// @brief Describes an instruction so lowering can be accurate.
class TIRInstInfo {
    /// @brief Implicit operands, such as registers.
    ///
    /// For example on x86:
    /// The instruction mul has the AL family and the DL family as implicit
    /// registers. This isn't only dest but is also src, since some functions
    /// have src as implicit. This is once again mainly an x86 thing, so for
    /// example div/idiv have those. If the operand is not overriden but you
    /// need another index, use TIROperand() empty constructor.
    ncarrview<TIROperand> implicit_;
    bool enabled_; ///< Whether or not this instruction is present.
    /// @brief Which operands are allowed in what spot.
    ///
    /// Every instruction must assume memory operands are exclusive.
    /// Immediate x Immediate is not possible, thats also an assumption.
public:
    enum class OperandAllowed : uint8_t {
        None = 0x0,
        Reg = 0x1,
        Mem = 0x2,
        Imm = 0x4,
        // Shortcuts

        RegMem = Reg | Mem,
        RegMemImm = Reg | Mem | Imm,
        RegImm = Reg | Imm,
        MemImm = Mem | Imm
    };

private:
    /// @brief What operands are allowed in this instruction.
    ///
    /// Assume that registers are allowed, can be changed.
    /// @note 6 should be more than enough.
    OperandAllowed operandsAllowed_[6] = {
        OperandAllowed::Reg, OperandAllowed::Reg, OperandAllowed::Reg,
        OperandAllowed::Reg, OperandAllowed::Reg, OperandAllowed::Reg};
    bool allowImmToMem_ = false;

public:
    constexpr TIRInstInfo() noexcept : enabled_(true) {}

    constexpr TIRInstInfo(bool e) noexcept : enabled_(e) {}

    constexpr TIRInstInfo(ncarrview<TIROperand> implicit,
                          bool allowImmToMem = false) noexcept :
        implicit_(implicit), enabled_(true), allowImmToMem_(allowImmToMem) {}
    constexpr TIRInstInfo(ncarrview<TIROperand> implicit,
                          std::initializer_list<OperandAllowed> opAllow,
                          bool allowImmToMem = false) noexcept :
        implicit_(implicit), enabled_(true), allowImmToMem_(allowImmToMem) {
        for(auto it = opAllow.begin(); it != opAllow.end(); ++it) {
            unsigned idx = it - opAllow.begin();
            if(idx == sizeof(operandsAllowed_)) break;
            operandsAllowed_[idx] = *it;
        }
    }

    constexpr bool getEnabled() const noexcept {
        return enabled_;
    }

    constexpr bool operandOverriden(unsigned n) const noexcept {
        return (n < implicit_.size()) && (!implicit_[n].isNone());
    }

    constexpr OperandAllowed checkOperand(unsigned n) const noexcept {
        return n < sizeof(operandsAllowed_) ? operandsAllowed_[n]
                                            : OperandAllowed::None;
    }

    constexpr bool isDestOverriden() const noexcept {
        return operandOverriden(0);
    }

    constexpr arrview<TIROperand> getImplicit() const noexcept {
        return implicit_;
    }

    constexpr ncarrview<TIROperand> getImplicit() noexcept {
        return implicit_;
    }

    constexpr bool immToMemAllowed() const noexcept {
        return allowImmToMem_;
    }
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

class TIRSub : public TIRInstruction {
public:
    TIRSub(TIRBlock* parent, const Type* type, TIROperand* dest,
           TIROperand* src) :
        TIRInstruction(parent, type, {dest, src}, TIRInstID::SUB_DEST_SRC) {}

    TIRSub(TIRBlock* parent, const Type* type, TIROperand* dest,
           TIROperand* src1, TIROperand* src2) :
        TIRInstruction(parent, type, {dest, src1, src2},
                       TIRInstID::SUB_DEST_SRC_SRC) {}
};

class TIRRet : public TIRInstruction {
public:
    TIRRet(TIRBlock* parent, const Type* type) :
        TIRInstruction(parent, type, {}, TIRInstID::RET) {}
};

} // namespace inr

#endif // INERTIA_TIR_TIRINSTRUCTION_H
