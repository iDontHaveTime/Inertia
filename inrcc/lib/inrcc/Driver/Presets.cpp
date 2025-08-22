#include "Inertia/Version.hpp"
#include "inrcc/Driver/Presets.hpp"
#include "inrcc/Driver/Defaults.hpp"
#include <iostream>

namespace inrcc{
namespace presets{
void print_default_linker(void){
    std::cout<<"Default linker: "<<defaults::default_linker.getName()<<'\n';
}
void print_default_triple(void){
    std::cout<<"Default triple: "<<defaults::default_triple.getLoadedString()<<'\n';
}
void print_version(void){
    std::cout<<"inrcc version "<<Inertia::InertiaVersion<<'\n';
}
}
}
