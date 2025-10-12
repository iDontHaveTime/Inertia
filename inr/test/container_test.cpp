#include "inr/Support/inrstream.hpp"
#include "inr/Support/inrvector.hpp"

int main(){

    inr::inr_vec<int> vec;
    vec.emplace_back(2);

    if(vec.begin() < vec.end()){
        inr::out<<"vector begin is less than vector end"<<inr::endl;
    }

    for(int& i : vec){
        inr::out<<i<<inr::endl;
    }
    return 0;
}