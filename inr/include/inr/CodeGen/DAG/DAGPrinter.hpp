#ifndef INERTIA_DAGPRINTER_HPP
#define INERTIA_DAGPRINTER_HPP

/**
 * @file inr/CodeGen/DAG/DAGPrinter.hpp
 * @brief Header for the DAG printer class.
 *
 * This header contains the Inertia's DAG printer class.
 *
 **/

#include "inr/CodeGen/DAG/DAGNode.hpp"
#include "inr/CodeGen/DAG/SelectionDAG.hpp"
#include "inr/Support/Alloc.hpp"
#include "inr/Support/Stream.hpp"
#include "inr/Support/Vector.hpp"

namespace inr{

    constexpr const char* opcode_to_str(dag::DAGOpcode op) noexcept{
        switch(op){
            case dag::DAGOpcode::ADD: return "Add";
            case dag::DAGOpcode::SUB: return "Sub";
            case dag::DAGOpcode::MUL: return "Mul";
            case dag::DAGOpcode::SDIV: return "SDiv";
            case dag::DAGOpcode::UDIV: return "UDiv";
            case dag::DAGOpcode::LOAD: return "Load";
            case dag::DAGOpcode::STORE: return "Store";
            case dag::DAGOpcode::AND: return "And";
            case dag::DAGOpcode::OR: return "Or";
            case dag::DAGOpcode::XOR: return "Xor";
            case dag::DAGOpcode::NOT: return "Not";
            case dag::DAGOpcode::CONSTANT: return "Const";
            case dag::DAGOpcode::ENTRY: return "Entry";
            default: return "Unknown";
        }
    }

    /**
     * @brief This class prints out the DAG nodes.
     */
    class DAGPrinter{
    
    public:
        DAGPrinter() noexcept = default;

        /**
         * @brief This prints out the contents of the DAG linearly.
         */
        template<inertia_allocator _al>
        void print(inr::inr_ostream& os, const inr::dag::SelectionDAG<_al>& dag) noexcept{
            for(const inr::dag::Node<_al>* nd : dag.get_nodes()){
                os<<'x'<<nd->value_id<<": "<<opcode_to_str(nd->op);

                if(nd->get_type()){
                    os<<" : "<<*nd->get_type();
                }

                if(!nd->operands.empty()){
                    os<<" (";
                    for(size_t i = 0; i < nd->operands.size(); i++){
                        if(i > 0){
                            os<<", ";
                        }
                        os<<'x'<<nd->operands[i]->value_id;
                    }
                    os<<')';
                }

                if(nd->op == dag::DAGOpcode::CONSTANT){
                    os<<" = "<<((const dag::ConstNode<_al>*)nd)->val;
                }

                os<<'\n';
            }
        }

        /**
         * @brief Prints out the dag as a tree.
         */
        template<inertia_allocator _na>
        void print(inr_ostream& os, const dag::Node<_na>* nd) noexcept{
            if(!nd) return;

            inr_vec<bool> drawBar;
            print_node(os, nd, drawBar, true);
        }

    private:

        template<inertia_allocator _na>
        void print_node(inr_ostream& os, const dag::Node<_na>* node, inr_vec<bool>& drawBar, bool isLast){
            if(!node) return;

            for(bool draw : drawBar){
                os<<(draw ? '|' : ' ')<<"  ";
            }

            os<<"+- ";

            os<<'x'<<node->value_id<<": "<<opcode_to_str(node->op);
            if(node->op == dag::DAGOpcode::CONSTANT){
                os<<" = "<<((const dag::ConstNode<_na>*)node)->val;
            }
            os<<'\n';

            drawBar.push_back(!isLast);
            for(size_t i = 0; i < node->operands.size(); i++){
                print_node(os, node->operands[i], drawBar, i + 1 == node->operands.size());
            }

            drawBar.pop_back();
        }

    public:

        ~DAGPrinter() noexcept = default;
    };

}

#endif // INERTIA_DAGPRINTER_HPP
