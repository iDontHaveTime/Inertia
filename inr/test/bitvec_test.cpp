#include "inr/Support/BitVector.hpp"
#include "inr/Support/Stream.hpp"
#include <iostream>

int main(){
    inr::bit_vector vec;

    inr::out<<vec.capacity()<<inr::endl;
    inr::out<<vec.size()<<inr::endl;
    inr::out<<vec<<inr::endl;

    vec.set_bits(129);

    inr::out<<vec.capacity()<<inr::endl;
    inr::out<<vec.size()<<inr::endl;
    inr::out<<vec<<inr::endl;

    std::cout<<vec<<std::endl;

    return 0;
}