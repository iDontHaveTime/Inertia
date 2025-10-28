#include "inr/MC/MCELFWriter.hpp"
#include "inr/MC/MCOBJ.hpp"
#include "inr/Support/Stream.hpp"
#include "inr/Target/Triple.hpp"

int main(){
    inr::inr_ostream out("inr/test/resources/elftest.o");

    inr::Triple tt{inr::Triple::x86_64, inr::Triple::linux_, inr::Triple::gnu};

    inr::MCOBJ obj{tt};

    inr::MCELFWriter elf{obj, out};

    elf.write();

    return 0;
}