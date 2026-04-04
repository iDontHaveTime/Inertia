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

enum class DAGType : uint32_t {
    EntryToken, ///< Start of the DAG.

    // Leaves

    Block,
    Constant,
    Register,
    FrameIndex,
    CopyFromReg,

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

    DAG_TYPE_END
};

/// @brief DAG Node class.
/// @note Does NOT own the nodes unlike other classes using ilist.
class DAGNode {
    uint32_t nodeType_;
    ivec<DAGNode*, 3> operands_;
    const Type* type_;

public:
    DAGNode(uint32_t nodeType, const Type* type) noexcept :
        nodeType_(nodeType), type_(type) {}

    DAGNode(DAGType nodeType, const Type* type) noexcept :
        DAGNode(uint32_t(nodeType), type) {}

    const Type* getType() const noexcept {
        return type_;
    }

    uint32_t getNodeType() const noexcept {
        return nodeType_;
    }

    const ivec<DAGNode*, 3>& getOperands() const noexcept {
        return operands_;
    }

    DAGNode* getOperand(uint32_t i) const noexcept {
        return operands_[i];
    }

    void addOperand(DAGNode* node) noexcept {
        operands_.emplace_back(node);
    }

    bool isReg() const noexcept {
        switch(nodeType_) {
            case(uint32_t)DAGType::CopyFromReg:
                [[fallthrough]];
            case(uint32_t)DAGType::Register:
                return true;
            default:
                return false;
        }
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
    DAGRegister(DAGType dt, const Type* type, Register reg) noexcept :
        DAGNode(dt, type), reg_(reg) {}

    Register getRegister() const noexcept {
        return reg_;
    }
};

} // namespace inr

#endif // INERTIA_DAG_DAGNODE_H
