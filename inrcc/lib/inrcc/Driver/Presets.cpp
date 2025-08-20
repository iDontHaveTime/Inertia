#include "inrcc/Driver/Presets.hpp"
#include "inrcc/Driver/Defaults.hpp"
#include <iostream>

namespace inrcc{
namespace presets{
void print_default_linker(void){
    std::cout<<"Default linker: "<<defaults::default_linker.getName()<<'\n';
}
}
}
