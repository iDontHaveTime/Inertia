#include "inr/CodeGen/DAG/SelectionDAG.hpp"
#include "inr/CodeGen/DAG/DAGPrinter.hpp"
#include "inr/IR/Context.hpp"
#include "inr/Support/Int.hpp"
#include "inr/Support/Stream.hpp"


int main(){

    inr::inrContext ctx;
    inr::dag::SelectionDAG sdag;

    auto* five = sdag.create_constant(ctx.get_integer(32), inr::inrint(5, 32, false));
    auto* ten = sdag.create_constant(ctx.get_integer(32), inr::inrint(10, 32, false));
    auto* three = sdag.create_constant(ctx.get_integer(32), inr::inrint(3, 32, false));

    auto* sum = sdag.create_add(ctx.get_integer(32), five, ten);
    auto* result = sdag.create_sub(ctx.get_integer(32), sum, three);

    auto* entry = sdag.create_entry(result);

    inr::DAGPrinter dp;

    inr::out<<"Linear:\n";
    dp.print(inr::out, sdag);
    inr::out<<"Tree:\n";
    dp.print(inr::out, entry);

    return 0;
}