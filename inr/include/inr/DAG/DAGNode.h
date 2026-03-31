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

#include <cstdint>

namespace inr {

enum class DAGType : int32_t {
    EntryToken, ///< Start of the DAG.

    // Leaves

    Block,
    Constant,
    Register,
    FrameIndex,
    CopyToReg,
    CopyFromReg,

    // Integer

    ADD,  ///< Addition
    SUB,  ///< Subtraction
    MUL,  ///< Multiplication
    SDIV, ///< Signed Division
    UDIV, ///< Unsigned Division
    SREM, ///< Signed Remainder (Modulo)
    UREM, ///< Unsigned Remainder (Modulo)

    DAG_TYPE_END
};

/// @brief DAG Node class.
/// @note Does NOT own the nodes unlike other classes using ilist.
class DAGNode : public ilist_node<DAGNode> {
    int32_t nodeType_;
    ivec<DAGNode*, 3> operands_;
    const Type* type_;

public:
    DAGNode(int32_t nodeType, const Type* type) noexcept :
        nodeType_(nodeType), type_(type) {}

    DAGNode(DAGType nodeType, const Type* type) noexcept :
        DAGNode(int32_t(nodeType), type) {}

    const Type* getType() const noexcept {
        return type_;
    }

    int32_t getNodeType() const noexcept {
        return nodeType_;
    }

    const ivec<DAGNode*, 3>& getOperands() const noexcept {
        return operands_;
    }

    void addOperand(DAGNode* node) noexcept {
        operands_.emplace_back(node);
    }

    virtual ~DAGNode() noexcept = default;
};

class DAGConst : public DAGNode {
    const ConstantInt* value_;

public:
    DAGConst(const ConstantInt* value) noexcept :
        DAGNode(DAGType::Constant, value->getType()), value_(value) {}

    const ConstantInt* getValue() const noexcept {
        return value_;
    }
};

} // namespace inr

#endif // INERTIA_DAG_DAGNODE_H
