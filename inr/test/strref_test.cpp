#include "inr/Support/Stream.hpp"
#include "inr/Support/StringRef.hpp"

int main(){

    inr::strref str("Hello, World!");

    inr::out<<str<<inr::endl;

    return 0;
}