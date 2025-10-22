#include "inr/Support/inrstream.hpp"
#include "inr/Support/inrvector.hpp"

/**
 * Expected output:
 * vector begin is less than vector end
 * 2
 *
 */

int main(){

    /* Make a new integer vector. */
    inr::inr_vec<int> vec;
    /* Put 2 into the back. */
    vec.emplace_back(2);

    /* Confirm the iterators are fine. */
    if(vec.begin() < vec.end()){
        inr::out<<"vector begin is less than vector end"<<inr::endl;
    }

    /* Iterate and print through integers. */
    for(int& i : vec){
        inr::out<<i<<inr::endl;
    }

    return 0;
}