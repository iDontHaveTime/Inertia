#include "inr/Support/inralloc.hpp"
#include "inr/Support/inrptr.hpp"
#include "inr/Support/inrstream.hpp"

int main(){

    inr::unique<int> x = inr::static_allocator.make_unique<int>(42);

    inr::out<<*x<<inr::endl;

    inr::unique<int[]> y = inr::static_allocator.make_unique<int[]>(3, 52);

    for(size_t i = 0; i < y.size(); i++){
        inr::out<<y[i]<<inr::endl;
    }

    return 0;
}