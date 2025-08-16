#include "Inertia/Assembly/x86/x86ASMPrinter.hpp"
#include "Inertia/Assembly/Generic/ASMPrinterGen.hpp"
#include <fstream>
#include <ostream>
#include <sstream>

namespace Inertia{

bool x86ASMPrinter::output(const LoweredOutput& lowout, PrintingType pt){
    if(out.empty()) return true;

    std::stringstream _ss_;
    std::ofstream _of_(out);
    if(!_of_) return true;

    std::ostream& os = pt == PrintingType::FILEIO ? (std::ostream&)_of_ : (std::ostream&)_ss_;

    if(pt == PrintingType::MEMORYIO){
        _of_<<_ss_.str(); // dump the buffer to file
    }
    return false;
}

}