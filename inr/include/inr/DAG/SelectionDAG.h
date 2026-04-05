// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_DAG_SELECTIONDAG_H
#define INERTIA_DAG_SELECTIONDAG_H

/// @file DAG/SelectionDAG.h
/// @brief Provides the Selection DAG class.

#include <inr/DAG/DAGNode.h>
#include <inr/IR/Constant.h>
#include <inr/IR/Type.h>
#include <inr/MIR/Register.h>

#include <vector>

namespace inr {

/// @brief Own DAG nodes.
class SelectionDAG {
    std::vector<DAGNode*> nodes_;
    DAGNode* root_;
    DAGNode* entry_;
    DAGNode* chain_;

    DAGNode* addNode(DAGNode* node) {
        return nodes_.emplace_back(node);
    }

    template<typename T, typename... Args>
    DAGNode* newNode(Args&&... args) {
        return addNode(new T(std::forward<Args>(args)...));
    }

public:
    SelectionDAG() noexcept {
        root_ = entry_ = chain_ =
            newNode<DAGNode>(DAGType::EntryToken, nullptr);
    }

    SelectionDAG(const SelectionDAG&) = delete;
    SelectionDAG& operator=(const SelectionDAG&) = delete;

    SelectionDAG(SelectionDAG&&) noexcept = default;
    SelectionDAG& operator=(SelectionDAG&&) noexcept = default;

    ~SelectionDAG() noexcept {
        for(auto it = nodes_.rbegin(); it != nodes_.rend(); ++it) {
            delete *it;
        }
    }

    // BUILDING DAG

    DAGValue createConstant(const ConstantInt* value) {
        return DAGValue(newNode<DAGConst>(value));
    }

    DAGValue createAdd(const Type* type, DAGValue lhs, DAGValue rhs) {
        DAGNode* node = newNode<DAGNode>(DAGType::ADD, type);
        node->addOperand(lhs);
        node->addOperand(rhs);
        return DAGValue(node);
    }

    DAGValue createCopyFromReg(DAGValue dest, Register src, const Type* type) {
        DAGNode* node = newNode<DAGNode>(DAGType::CopyFromReg, type);
        node->addOperand(dest);
        node->addOperand(newNode<DAGRegister>(type, src));
        return DAGValue(node);
    }

    DAGValue createCopyToReg(DAGValue src, Register dest, const Type* type) {
        DAGNode* node = newNode<DAGNode>(DAGType::CopyToReg, type);
        node->addOperand(src);
        node->addOperand(newNode<DAGRegister>(type, dest));
        return DAGValue(node);
    }

    DAGValue createStore(const Type* type, DAGValue ptr, DAGValue value,
                         DAGValue chIn) {
        DAGNode* node = newNode<DAGNode>(DAGType::STORE, type);
        node->addOperand(ptr);
        node->addOperand(value);
        node->addOperand(chIn);

        return DAGValue(node);
    }

    DAGValue createTarget(uint32_t opcode, const Type* t) {
        return DAGValue(newNode<DAGNode>(opcode, t));
    }

    DAGValue getRoot() const noexcept {
        return DAGValue(root_);
    }

    DAGValue getEntry() const noexcept {
        return DAGValue(entry_);
    }

    DAGValue getChain() const noexcept {
        return DAGValue(chain_);
    }

    void setRoot(DAGValue node) noexcept {
        root_ = node.getNode();
    }

    void setChain(DAGValue node) noexcept {
        chain_ = node.getNode();
    }

    DAGValue createRegister(Register reg, const Type* type) {
        return DAGValue(newNode<DAGRegister>(type, reg));
    }
};

} // namespace inr

#endif // INERTIA_DAG_SELECTIONDAG_H
