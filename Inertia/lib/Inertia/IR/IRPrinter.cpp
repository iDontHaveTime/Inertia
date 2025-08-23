#include "Inertia/IR/IRPrinter.hpp"
#include "Inertia/IR/Block.hpp"
#include "Inertia/IR/Frame.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/IR/Instruction.hpp"
#include "Inertia/IR/Type.hpp"
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

bool PrintType(const Type* type, std::ostream& os){
    switch(type->getKind()){
        case Type::INTEGER:
            os<<"int<";
            os<<((IntegerType*)type)->width;
            os<<'>';
            break;
        case Type::FLOAT:
            switch(((FloatType*)type)->accuracy){
                case FloatType::DOUBLE_ACC:
                    os<<"double";
                    break;
                case FloatType::FLOAT_ACC:
                    [[fallthrough]];
                default:
                    os<<"float";
                    break;
            }
            break;
        case Type::POINTER:
            PrintType(((PointerType*)type)->pointee, os);
            os<<'*';
            break;
        case Type::VOID:
            [[fallthrough]];
        default:
            os<<"void";
            break;
    }

    return false;
}

bool PrintFunctionFlags(const Function& func, std::ostream& os){

    if(func.check_flag(Function::MANUAL_ALIGN)){
        os<<"align<"<<func.align.getValue()<<"> ";
    }

    if(func.check_flag(Function::LOCAL)){
        os<<"local ";
    }

    return false;
}

bool PrintRetInstruction(const IRReturn* ins, std::ostream& os){
    os<<"ret ";

    PrintType(ins->src->type, os);
    os<<' ';

    switch(ins->src.get()->ssa_type){
        case SSAType::NORMAL:
            os<<'%'<<ins->src->id;
            break;
        case SSAType::CONSTANT:
            os<<((SSAConst*)ins->src.get())->value;
            break;
    }

    return false;
}

bool PrintInstruction(const ArenaReference<IRInstruction>& ins, std::ostream& os){
    switch(ins->op){
        case IROpType::Ret:
            return PrintRetInstruction((const IRReturn*)ins.get(), os);
        default:
            return false;
    }
    return false;
}

bool PrintBlock(const Block& block, std::ostream& os){
    os<<block.name<<":\n";

    for(const ArenaReference<IRInstruction>& ins : block.instructions){
        os<<'\t';
        PrintInstruction(ins, os);
        os<<'\n';
    }

    return false;
}

bool PrintFunction(const Function& func, std::ostream& os){
    os<<"func ";

    PrintType(func.type, os);
    os<<' ';
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
        _of_<<_ss_.rdbuf(); // dump the buffer to file
    }
    return false;
}

}
