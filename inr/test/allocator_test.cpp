#include "inr/Support/inralloc.hpp"
#include "inr/Support/inrstream.hpp"

int main(){

    inr::basic_allocator alloc;

    int* x = alloc.alloc<int>();

    *x = 20;

    inr::out<<*x<<inr::endl;

    alloc.free(x);

    return 0;
}