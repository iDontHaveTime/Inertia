#include "inr/CodeGen/DAG/SelectionDAG.hpp"
#include "inr/IR/Context.hpp"
#include "inr/Support/Arena.hpp"
#include "inr/Support/Int.hpp"
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

    return 0;
}