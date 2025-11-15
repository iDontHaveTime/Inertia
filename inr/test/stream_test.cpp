#include "inr/Defines/FileDef.hpp"
#include "inr/Support/File.hpp"
#include "inr/Support/Byte.hpp"
#include "inr/Support/Stream.hpp"
#include <iostream>

int main(){
    
    inr::out<<"This is a message."<<inr::endl;

    inr::err<<"This is an error."<<inr::endl;

    inr::inrfile stdout_inr(1, BUFSIZ, inr::fs::OpeningType::Write, false);
    inr::inr_ostream inrout(std::move(stdout_inr));

    inrout<<"Hello from POSIX."<<inr::endl;

    inrout<<"Enter a string: "<<inr::flush;

    std::string str;
    inr::in.getline(str);

    inr::out<<str<<inr::endl;

    inr::byte b = 'A';

    inr::out<<b<<inr::endl;
    std::cout<<b<<std::endl;

    (inr::out<<"UTF: ").put(U'🦄')<<inr::endl;

    return 0;
}