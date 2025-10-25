#include "inr/Support/inrmap.hpp"
#include "inr/Support/inrstream.hpp"
#include <cstddef>

int main(){

    inr::inr_map<size_t, const char*> map;

    map[1] = "Hello";

    const char** str = map.get(1);

    if(str){
        inr::out<<*str<<inr::endl;
    }
    else{
        inr::out<<"No str found."<<inr::endl;
    }

    const char** no_str = map.get(0);

    if(no_str){
        inr::out<<"Huh, what?"<<inr::endl;
    }
    else{
        inr::out<<"No string at index 0."<<inr::endl;
    }

    for(size_t i = 0; i < 24; i++){
        map[i] = "Yo";
    }
    
    size_t correct = 0;
    for(size_t i = 0; i < 24; i++){
        const char** _ns = map.get(i);
        if(_ns){
            correct++;
        }
    }

    inr::out<<"Correct should be 24. Its: "<<correct<<inr::endl;

    return 0;
}