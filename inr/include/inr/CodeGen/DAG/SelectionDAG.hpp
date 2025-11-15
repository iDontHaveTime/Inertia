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
#include "inr/Support/Alloc.hpp"
#include "inr/Support/Vector.hpp"

#include <cstdint>

namespace inr::dag{

    template<inertia_allocator _sdag_alloc_ = allocator>
    class SelectionDAG : private _sdag_alloc_{
        inr_vec<Node<_sdag_alloc_>*, _sdag_alloc_> nodes;
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
         */
        SelectionDAG() noexcept : nodes(), next_value_id(0){};

        /**
         * @brief Creates a new ADD node.
         */
        Node<_sdag_alloc_>* create_add(type* node_type, Node<_sdag_alloc_>* lhs, Node<_sdag_alloc_>* rhs){
            Node<_sdag_alloc_>* node = _sdag_alloc_::template alloc<Node<_sdag_alloc_>>(DAGOpcode::ADD, node_type, next_value_id++);
            node->add_operand(lhs);
            node->add_operand(rhs);
            nodes.push_back(node);
            return node;
        }

        /**
         * @brief Creates a new SUB node.
         */
        Node<_sdag_alloc_>* create_sub(type* node_type, Node<_sdag_alloc_>* lhs, Node<_sdag_alloc_>* rhs){
            Node<_sdag_alloc_>* node = _sdag_alloc_::template alloc<Node<_sdag_alloc_>>(DAGOpcode::SUB, node_type, next_value_id++);
            node->add_operand(lhs);
            node->add_operand(rhs);
            nodes.push_back(node);
            return node;
        }

        /**
         * @brief Creates a new CONSTANT node.
         */
        Node<_sdag_alloc_>* create_constant(type* node_type, const inrint<_sdag_alloc_>& value){
            Node<_sdag_alloc_>* node = _sdag_alloc_::template alloc<ConstNode<_sdag_alloc_>>(value, node_type, next_value_id++);
            nodes.push_back(node);
            return node;
        }

        /**
         * @brief Creates a new CONSTANT node. Moves the integer, so its more efficient.
         */
        Node<_sdag_alloc_>* create_constant(type* node_type, inrint<_sdag_alloc_>&& value){
            Node<_sdag_alloc_>* node = _sdag_alloc_::template alloc<ConstNode<_sdag_alloc_>>(std::move(value), node_type, next_value_id++);
            nodes.push_back(node);
            return node;
        }

        /**
         * @brief Creates an ENTRY node (marks the root/result).
         */
        Node<_sdag_alloc_>* create_entry(Node<_sdag_alloc_>* result_node){
            Node<_sdag_alloc_>* node = _sdag_alloc_::template alloc<Node<_sdag_alloc_>>(DAGOpcode::ENTRY, nullptr, next_value_id++);
            node->add_operand(result_node);
            nodes.push_back(node);
            return node;
        }

        /**
         * @brief Gets the entry node (root of DAG).
         */
        Node<_sdag_alloc_>* get_entry() noexcept{
            for(Node<_sdag_alloc_>* n : nodes){
                if(n->op == DAGOpcode::ENTRY){
                    return n;
                }
            }
            return nullptr;
        }

        /**
         * @brief Gets all nodes in the DAG.
         */
        inr_vec<Node<_sdag_alloc_>*>& get_nodes() noexcept{
            return nodes;
        }

        /**
         * @brief Gets all nodes in the DAG.
         */
        const inr_vec<Node<_sdag_alloc_>*>& get_nodes() const noexcept{
            return nodes;
        }
    };
    
}

#endif // INERTIA_SELECTIONDAG_HPP
