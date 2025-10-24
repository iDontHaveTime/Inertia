#ifndef INERTIA_DAGNODE_HPP
#define INERTIA_DAGNODE_HPP

/**
 * @file inr/CodeGen/DAG/DAGNode.hpp
 * @brief Header for the DAG Node class.
 *
 * This header contains the Inertia's DAG Node class.
 *
 **/

#include "inr/Support/inralloc.hpp"
#include "inr/Support/inrint.hpp"
#include "inr/Support/inrvector.hpp"

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
    class Node{
    public:
        inline_vec<Node*, 4> operands;
        const uint32_t value_id;
        const DAGOpcode op;

        /**
         * @brief Creates a new DAG node.
         *
         * @param _op DAG Node type.
         * @param id The ID the node uses.
         * @param _mem The allocator operands vector will use. Not relevant on stack.
         */
        Node(DAGOpcode _op, uint32_t id, allocator* _mem = nullptr) noexcept : operands(_mem), value_id(id), op(_op){};

        void add_operand(Node* node){
            operands.push_back(node);
        }
    };

    class ConstNode : public Node{
    public:
        inrint val;
        ConstNode(const inrint& i, uint32_t id, allocator* _mem = nullptr) noexcept : Node(DAGOpcode::CONSTANT, id, _mem), val(i){};
        ConstNode(inrint&& i, uint32_t id, allocator* _mem = nullptr) noexcept : Node(DAGOpcode::CONSTANT, id, _mem), val(std::move(i)){};
    };
}

#endif // INERTIA_DAGNODE_HPP
