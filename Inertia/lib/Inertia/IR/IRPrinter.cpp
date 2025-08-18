#include "Inertia/IR/IRPrinter.hpp"
#include "Inertia/IR/Block.hpp"
#include "Inertia/IR/Frame.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>

namespace Inertia{

bool PrintTriple(const Frame& frame, std::ostream& os){
    os<<"triple = \""<<frame.ttriple->getLoadedString()<<"\"\n";
    return false;
}

bool PrintFunctionFlags(const Function& func, std::ostream& os){

    if(func.check_flag(Function::MANUAL_ALIGN)){
        os<<"align<"<<std::to_string(func.align.getValue())<<"> ";
    }

    if(func.check_flag(Function::LOCAL)){
        os<<"local ";
    }

    return false;
}

bool PrintBlock(const Block& block, std::ostream& os){
    os<<block.name<<":\n";

    return false;
}

bool PrintFunction(const Function& func, std::ostream& os){
    os<<"func ";
    PrintFunctionFlags(func, os);

    os<<'@'<<func.name<<'(';

    os<<")\n";

    for(const ArenaReference<Block>& block : func.blocks){
        PrintBlock(*block.get(), os);
    }

    os<<"end\n";
    
    return false;
}

bool IRPrinter::output(const Frame& frame, PrintingType pt){
    if(!frame.ttriple) return true;
    
    std::stringstream _ss_;
    std::ofstream _of_(out);
    if(!_of_) return true;

    std::ostream& os = pt == PrintingType::FILEIO ? (std::ostream&)_of_ : (std::ostream&)_ss_;

    PrintTriple(frame, os);
    os<<'\n';

    for(const Function& func : frame.funcs){
        PrintFunction(func, os);
        os<<'\n';
    }

    if(pt == PrintingType::MEMORYIO){
        _of_<<_ss_.str(); // dump the buffer to file
    }
    return false;
}

}