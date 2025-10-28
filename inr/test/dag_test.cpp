#include "inr/CodeGen/DAG/DAGNode.hpp"
#include "inr/CodeGen/DAG/SelectionDAG.hpp"
#include "inr/IR/Context.hpp"
#include "inr/Support/Arena.hpp"
#include "inr/Support/Int.hpp"
#include "inr/Support/Stream.hpp"

void print_dag(const inr::dag::SelectionDAG& dag){
    inr::out<<"--- DAG Nodes ---\n";
    
    for(const inr::dag::Node* nd : dag.get_nodes()){
        inr::out<<'x'<<nd->value_id<<": ";

        switch(nd->op){
            case inr::dag::DAGOpcode::ADD:
                inr::out<<"Add x"<<nd->operands[0]->value_id<<", x"<<nd->operands[1]->value_id<<'\n';
                break;
            case inr::dag::DAGOpcode::SUB:
                inr::out<<"Sub x"<<nd->operands[0]->value_id<<", x"<<nd->operands[1]->value_id<<'\n';
                break;
            case inr::dag::DAGOpcode::CONSTANT:
                inr::out<<"Const("<<((inr::dag::ConstNode*)nd)->val<<")\n";
                break;
            case inr::dag::DAGOpcode::ENTRY:
                inr::out<<"Entry x"<<nd->operands[0]->value_id<<'\n';
                break;
            default:
                inr::out<<"???\n";
                break;
        }
    }
    inr::flush(inr::out);
}

void emit_node(inr::dag::Node* node){
    switch(node->op){
        case inr::dag::DAGOpcode::CONSTANT:{
                auto* cn = (const inr::dag::ConstNode*)node;
                inr::out<<"\tmovl $"<<cn->val<<", %eax\n";
                break;
            }
        case inr::dag::DAGOpcode::ADD:{
                emit_node(node->operands[0]);
                if(node->operands[1]->op == inr::dag::DAGOpcode::CONSTANT){
                    auto* cn = (const inr::dag::ConstNode*)node->operands[1];
                    inr::out<<"\taddl $"<<cn->val<<", %eax\n";
                } 
                else{
                    inr::out<<"\t# TODO: handle non-constant right operand\n";
                }
                break;
            }
        case inr::dag::DAGOpcode::SUB:{
                emit_node(node->operands[0]);
                if(node->operands[1]->op == inr::dag::DAGOpcode::CONSTANT){
                    auto* cn = (inr::dag::ConstNode*)node->operands[1];
                    inr::out<<"\tsubl $"<<cn->val<<", %eax\n";
                }
                break;
            }
        default:
            break;
    }
}

int main(){
    inr::arena_allocator<0x10000> arena;

    inr::inrContext ctx;
    inr::dag::SelectionDAG sdag(&arena);

    auto* five = sdag.create_constant(ctx.get_integer(32), inr::inrint(5, 32, false));
    auto* ten = sdag.create_constant(ctx.get_integer(32), inr::inrint(10, 32, false));
    auto* three = sdag.create_constant(ctx.get_integer(32), inr::inrint(3, 32, false));

    auto* sum = sdag.create_add(ctx.get_integer(32), five, ten);
    auto* result = sdag.create_sub(ctx.get_integer(32), sum, three);

    auto* entry = sdag.create_entry(result);

    print_dag(sdag);

    emit_node(entry->operands[0]);

    return 0;
}