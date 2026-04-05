// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_DAG_DAGNODE_H
#define INERTIA_DAG_DAGNODE_H

/// @file DAG/DAGNode.h
/// @brief Provides the DAG Node class.

#include <inr/ADT/IList.h>
#include <inr/ADT/IVector.h>
#include <inr/IR/Constant.h>
#include <inr/IR/Type.h>
#include <inr/MIR/Register.h>

#include <cstdint>

namespace inr {

/// @brief Opcode of the dag node.
enum class DAGType : uint32_t {
    EntryToken, ///< Start of the DAG.

    // Leaves

    Block,
    Constant,
    Register,
    FrameIndex,

    // Integer

    ADD,  ///< Addition
    SUB,  ///< Subtraction
    MUL,  ///< Multiplication
    SDIV, ///< Signed Division
    UDIV, ///< Unsigned Division
    SREM, ///< Signed Remainder (Modulo)
    UREM, ///< Unsigned Remainder (Modulo)

    // Other
    CopyToReg,
    CopyFromReg,

    STORE, ///< Store.

    DAG_TYPE_END
};

/// @brief Returns a dag node opcode as a string.
constexpr sview dagTypeToStr(DAGType);

class DAGNode;

/// @brief Wraps the dag node and also stores which result to use.
class DAGValue {
    DAGNode* node_ = nullptr;
    unsigned resN_ = 0;

public:
    /// @brief Default constructor.
    DAGValue() noexcept = default;

    /// @brief The usual constructor called by SelectionDAG.
    /// @param node The node to store.
    /// @param resN Result number.
    DAGValue(DAGNode* node, unsigned resN = 0) noexcept :
        node_(node), resN_(resN) {}

    DAGValue(const DAGValue&) noexcept = default;
    DAGValue(DAGValue&&) noexcept = default;

    DAGValue& operator=(const DAGValue&) noexcept = default;
    DAGValue& operator=(DAGValue&&) noexcept = default;

    /// @brief Returns the result number field.
    unsigned getResN() const noexcept {
        return resN_;
    }

    /// @brief Returns the node.
    DAGNode* getNode() const noexcept {
        return node_;
    }

    /// @brief Compares two values.
    bool operator==(const DAGValue& other) const noexcept {
        return other.node_ == node_ && other.resN_ == resN_;
    }
};

/// @brief Represents a use in dag.
class DAGUse {
    DAGValue value_;      ///< Node.
    DAGNode* user_;       ///< User.
    unsigned operandIdx_; ///< Operand index.

public:
    DAGUse(DAGValue value, DAGNode* user, unsigned idx) noexcept :
        value_(value), user_(user), operandIdx_(idx) {}

    DAGValue getValue() const noexcept {
        return value_;
    }

    DAGNode* getUser() const noexcept {
        return user_;
    }

    unsigned getOperandIdx() const noexcept {
        return operandIdx_;
    }
};

/// @brief DAG Node class.
/// @note Does NOT own the nodes unlike other classes using ilist.
class DAGNode {
    ivec<DAGNode*, 4> operands_;
    std::vector<DAGUse> uses_;
    const Type* type_;
    uint32_t nodeType_;

public:
    DAGNode(uint32_t nodeType, const Type* type) noexcept :
        type_(type), nodeType_(nodeType) {}

    DAGNode(DAGType nodeType, const Type* type) noexcept :
        DAGNode(uint32_t(nodeType), type) {}

    const Type* getType() const noexcept {
        return type_;
    }

    uint32_t getNodeType() const noexcept {
        return nodeType_;
    }

    const ivec<DAGNode*, 4>& getOperands() const noexcept {
        return operands_;
    }

    ivec<DAGNode*, 4>& getOperands() noexcept {
        return operands_;
    }

    DAGValue getOperandValue(unsigned idx) const noexcept {
        return DAGValue(operands_[idx]);
    }

    uint32_t getNumOperands() const noexcept {
        return operands_.size();
    }

    DAGNode* getOperand(uint32_t i) const noexcept {
        return operands_[i];
    }

    void addOperand(DAGValue value) {
        operands_.emplace_back(value.getNode());
        uses_.emplace_back(value, this, operands_.size() - 1);
    }

    bool isReg() const noexcept {
        return nodeType_ == (uint32_t)DAGType::Register;
    }

    bool isImm() const noexcept {
        switch(nodeType_) {
            case(uint32_t)DAGType::Constant:
                return true;
            default:
                return false;
        }
    }

    virtual ~DAGNode() noexcept = default;
};

/// @brief Constant integer DAG.
class DAGConst : public DAGNode {
    const ConstantInt* value_;

public:
    DAGConst(const ConstantInt* value) noexcept :
        DAGNode(DAGType::Constant, value->getType()), value_(value) {}

    const ConstantInt* getValue() const noexcept {
        return value_;
    }
};

/// @brief Anything register related DAG node.
class DAGRegister : public DAGNode {
    Register reg_;

public:
    DAGRegister(const Type* type, Register reg) noexcept :
        DAGNode(DAGType::Register, type), reg_(reg) {}

    Register getRegister() const noexcept {
        return reg_;
    }
};

constexpr sview dagTypeToStr(DAGType dt) {
    switch(dt) {
        case DAGType::EntryToken:
            return "EntryToken";
        case DAGType::Block:
            return "Block";
        case DAGType::Constant:
            return "Constant";
        case DAGType::Register:
            return "Register";
        case DAGType::FrameIndex:
            return "FrameIndex";
        case DAGType::ADD:
            return "ADD";
        case DAGType::SUB:
            return "SUB";
        case DAGType::MUL:
            return "MUL";
        case DAGType::SDIV:
            return "SDIV";
        case DAGType::UDIV:
            return "UDIV";
        case DAGType::SREM:
            return "SREM";
        case DAGType::UREM:
            return "UREM";
        case DAGType::CopyToReg:
            return "CopyToReg";
        case DAGType::CopyFromReg:
            return "CopyFromReg";
        case DAGType::STORE:
            return "STORE";
        case DAGType::DAG_TYPE_END:
            return "DAG_TYPE_END";
        default:
            return "target-specific";
    }
}

inline raw_stream& operator<<(raw_stream& os, DAGType dt) {
    return os << dagTypeToStr(dt);
}

} // namespace inr

#endif // INERTIA_DAG_DAGNODE_H
