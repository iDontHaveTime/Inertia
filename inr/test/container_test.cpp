#include "inr/Support/inrstream.hpp"
#include "inr/Support/inrvector.hpp"

/**
 * Expected output:
 * vector begin is less than vector end
 * 2
 * 67
 * 67
 * 128
 * Now heap:
 * 67
 * 128
 * 128
 * 128
 * 64
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

    /* Make an inline integer vector. */
    inr::inline_vec<int, 4> ivec;

    /* Emplace integer back. */
    ivec.emplace_back(67);

    for(int& i : ivec){
        inr::out<<i<<inr::endl;
    }

    ivec.emplace_back(128);

    for(int& i : ivec){
        inr::out<<i<<inr::endl;
    }

    ivec.push_back(128);
    ivec.emplace_back(128);
    ivec.emplace_back(sizeof(ivec));

    inr::out<<"Now heap:"<<inr::endl;

    for(int& i : ivec){
        inr::out<<i<<inr::endl;
    }

    return 0;
}