#ifndef INERTIA_SELECTIONDAG_HPP
#define INERTIA_SELECTIONDAG_HPP

/**
 * @file inr/CodeGen/DAG/SelectionDAG.hpp
 * @brief Header for the Selection DAG class.
 *
 * This header contains Inertia's Selection DAG class.
 *
 **/

#include "inr/CodeGen/DAG/DAGNode.hpp"
#include "inr/Support/inralloc.hpp"
#include "inr/Support/inrvector.hpp"

#include <cstdint>

namespace inr::dag{

    class SelectionDAG{
        inr_vec<Node*> nodes;
        uint32_t next_value_id;
    public:

        SelectionDAG(const SelectionDAG&) = delete;
        SelectionDAG& operator=(const SelectionDAG&) = delete;

        SelectionDAG(SelectionDAG&&) noexcept = default;
        SelectionDAG& operator=(SelectionDAG&&) noexcept = default;

        ~SelectionDAG() noexcept = default;

        /**
         * @brief Creates a new SelectionDAG.
         * 
         * @param _alloc The allocator to use for node allocation.
         */
        SelectionDAG(allocator* _alloc = nullptr) noexcept : nodes(_alloc), next_value_id(0){};

        /**
         * @brief Creates a new ADD node.
         */
        Node* create_add(type* node_type, Node* lhs, Node* rhs){
            Node* node = nodes.get_allocator()->alloc<Node>(DAGOpcode::ADD, node_type, next_value_id++, nodes.get_allocator());
            node->add_operand(lhs);
            node->add_operand(rhs);
            nodes.push_back(node);
            return node;
        }

        /**
         * @brief Creates a new SUB node.
         */
        Node* create_sub(type* node_type, Node* lhs, Node* rhs){
            Node* node = nodes.get_allocator()->alloc<Node>(DAGOpcode::SUB, node_type, next_value_id++, nodes.get_allocator());
            node->add_operand(lhs);
            node->add_operand(rhs);
            nodes.push_back(node);
            return node;
        }

        /**
         * @brief Creates a new CONSTANT node.
         */
        Node* create_constant(type* node_type, const inrint& value){
            Node* node = nodes.get_allocator()->alloc<ConstNode>(value, node_type, next_value_id++, nodes.get_allocator());
            nodes.push_back(node);
            return node;
        }

        /**
         * @brief Creates a new CONSTANT node. Moves the integer, so its more efficient.
         */
        Node* create_constant(type* node_type, inrint&& value){
            Node* node = nodes.get_allocator()->alloc<ConstNode>(std::move(value), node_type, next_value_id++, nodes.get_allocator());
            nodes.push_back(node);
            return node;
        }

        /**
         * @brief Creates an ENTRY node (marks the root/result).
         */
        Node* create_entry(Node* result_node){
            Node* node = nodes.get_allocator()->alloc<Node>(DAGOpcode::ENTRY, nullptr, next_value_id++, nodes.get_allocator());
            node->add_operand(result_node);
            nodes.push_back(node);
            return node;
        }

        /**
         * @brief Gets the entry node (root of DAG).
         */
        Node* get_entry() noexcept{
            for(Node* n : nodes){
                if(n->op == DAGOpcode::ENTRY){
                    return n;
                }
            }
            return nullptr;
        }

        /**
         * @brief Gets all nodes in the DAG.
         */
        inr_vec<Node*>& get_nodes() noexcept{
            return nodes;
        }

        /**
         * @brief Gets all nodes in the DAG.
         */
        const inr_vec<Node*>& get_nodes() const noexcept{
            return nodes;
        }
    };
    
}

#endif // INERTIA_SELECTIONDAG_HPP
