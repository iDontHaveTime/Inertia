#include "inr/Support/Alloc.hpp"
#include "inr/Support/Ptr.hpp"
#include "inr/Support/Stream.hpp"

int main(){
    inr::allocator alloc;

    inr::unique<int> x = alloc.make_unique<int>(42);

    inr::out<<*x<<inr::endl;

    inr::unique<int[], inr::allocator> y = alloc.make_unique<int[]>(3, 52);

    for(size_t i = 0; i < y.size(); i++){
        inr::out<<y[i]<<inr::endl;
    }

    return 0;
}