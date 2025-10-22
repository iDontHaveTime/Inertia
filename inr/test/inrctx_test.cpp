#include "inr/IR/inrcontext.hpp"
#include "inr/Support/inralloc.hpp"
#include "inr/Support/inrptr.hpp"

int main(){
    /* Create a new unique pointer context. */
    inr::unique<inr::inrContext> ctx = inr::static_allocator.make_unique<inr::inrContext>();

    return 0;
}