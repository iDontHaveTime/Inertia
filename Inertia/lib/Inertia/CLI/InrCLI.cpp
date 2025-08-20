#include "Inertia/CLI/InrCLI.hpp"
#include <iostream>

namespace Inertia{

bool parse_and_exec_cliarg(CLIFlag* arg){
    switch(arg->type){
        case InrCLIArg::None:
            return false;
        case InrCLIArg::Print:
            std::cout<<((CLIFlagPrintStr*)arg)->output;
            break;
        case InrCLIArg::Exec:
            ((CLIFlagExec*)arg)->exec();
            break;
    }
    return false;
}

bool InrCLI::parse_args(int argc, char** argv){
    if(argc == 1) return false;

    for(int i = 1; i < argc; i++){
        if(argv[i] == nullptr) return false;
        auto it = flags.find(argv[i]);
        if(it == flags.end()){
            continue;
        }
        if(parse_and_exec_cliarg(it->second)){
            return true;
        }
    }

    return false;
}

}
