#include "inr/Defines/inrfiledef.hpp"
#include "inr/Support/inrfile.hpp"
#include "inr/Support/inrstream.hpp"

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

    return 0;
}