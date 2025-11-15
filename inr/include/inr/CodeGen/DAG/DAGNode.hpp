#ifndef INERTIA_DAGNODE_HPP
#define INERTIA_DAGNODE_HPP

/**
 * @file inr/CodeGen/DAG/DAGNode.hpp
 * @brief Header for the DAG Node class.
 *
 * This header contains the Inertia's DAG Node class.
 *
 **/

#include "inr/IR/Type.hpp"
#include "inr/Support/Alloc.hpp"
#include "inr/Support/Int.hpp"
#include "inr/Support/Vector.hpp"

#include <cstdint>

namespace inr::dag{

    /**
     * @brief Type of the DAG Node.
     */
    enum class DAGOpcode : uint16_t{
        /* Integer Arithmetic. */
        ADD,
        SUB,
        MUL,
        SDIV,
        UDIV,
        /* Memory. */
        LOAD,
        STORE,
        /* Bitwise. */
        AND,
        OR,
        XOR,
        NOT,
        /* Constants. */
        CONSTANT,
        /* Special. */
        ENTRY
    };

    /**
     * @brief The DAG Node class.
     */
    template<inertia_allocator _node_alloc_ = allocator>
    class Node{
    public:
        inline_vec<Node*, 4, _node_alloc_> operands;
        inline_vec<Node*, 4, _node_alloc_> users;
        type* node_type;
        const uint32_t value_id;
        const DAGOpcode op;

        /**
         * @brief Creates a new DAG node.
         *
         * @param _op DAG Node type.
         * @param id The ID the node uses.
         * @param _mem The allocator operands vector will use. Not relevant on stack.
         */
        Node(DAGOpcode _op, type* _node_type, uint32_t id) noexcept : operands(), node_type(_node_type), value_id(id), op(_op){};

        void add_operand(Node* node){
            operands.push_back(node);
            node->users.push_back(this);
        }

        const type* get_type() const noexcept{
            return node_type;
        }

        type* get_type() noexcept{
            return node_type;
        }
    };

    template<inertia_allocator _cnode_alloc_ = allocator>
    class ConstNode : public Node<_cnode_alloc_>{
    public:
        inrint<_cnode_alloc_> val;
        ConstNode(const inrint<_cnode_alloc_>& i, type* node_type, uint32_t id) noexcept : Node<_cnode_alloc_>(DAGOpcode::CONSTANT, node_type, id), val(i){};
        ConstNode(inrint<_cnode_alloc_>&& i, type* node_type, uint32_t id) noexcept : Node<_cnode_alloc_>(DAGOpcode::CONSTANT, node_type, id), val(std::move(i)){};
    };
}

#endif // INERTIA_DAGNODE_HPP
