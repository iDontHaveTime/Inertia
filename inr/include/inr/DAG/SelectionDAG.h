// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_DAG_SELECTIONDAG_H
#define INERTIA_DAG_SELECTIONDAG_H

/// @file DAG/SelectionDAG.h
/// @brief Provides the Selection DAG class.

#include <inr/DAG/DAGNode.h>
#include <inr/IR/Constant.h>

#include <vector>

namespace inr {

/// @brief Own DAG nodes.
class SelectionDAG {
    std::vector<DAGNode*> nodes_;

    DAGNode* addNode(DAGNode* node) {
        return nodes_.emplace_back(node);
    }

    template<typename T, typename... Args>
    DAGNode* newNode(Args&&... args) {
        return addNode(new T(std::forward<Args>(args)...));
    }

public:
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

    DAGNode* createConstant(const ConstantInt* value) {
        return newNode<DAGConst>(value);
    }

    DAGNode* createAdd(const Type* type, DAGNode* lhs, DAGNode* rhs) {
        DAGNode* node = newNode<DAGNode>(DAGType::ADD, type);
        node->addOperand(lhs);
        node->addOperand(rhs);
        return node;
    }
};

} // namespace inr

#endif // INERTIA_DAG_SELECTIONDAG_H
