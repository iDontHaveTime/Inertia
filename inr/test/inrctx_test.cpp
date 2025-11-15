#include "inr/IR/Context.hpp"
#include "inr/Support/Alloc.hpp"
#include "inr/Support/Ptr.hpp"

int main(){
    /* Create a new unique pointer context. */
    inr::allocator alloc;
    inr::unique<inr::inrContext<inr::allocator>, inr::allocator> ctx = alloc.make_unique<inr::inrContext<inr::allocator>>();

    return 0;
}