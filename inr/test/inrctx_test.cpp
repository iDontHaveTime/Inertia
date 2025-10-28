#include "inr/IR/Context.hpp"
#include "inr/Support/Alloc.hpp"
#include "inr/Support/Ptr.hpp"

int main(){
    /* Create a new unique pointer context. */
    inr::unique<inr::inrContext> ctx = inr::static_allocator.make_unique<inr::inrContext>();

    return 0;
}