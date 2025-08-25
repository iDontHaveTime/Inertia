#ifndef INERTIA_PROCESS_HPP
#define INERTIA_PROCESS_HPP

#include "Inertia/CLI/InrRun.hpp"
#include <vector>

namespace Inertia{
    class Process{
        std::vector<const char*> argv;
    public:

        void set_cmd(const char* name) noexcept{
            if(!argv.empty()){
                argv.front() = name;
            }
            else{
                argv.push_back(name);
            }
        }

        // does not set process name
        void add_arg(const char* arg){
            if(argv.empty()){
                argv.push_back(nullptr);
                argv.push_back(arg);
                return;
            }
            else if(argv.back() == nullptr && argv.size() > 1){
                argv.back() = arg;
            }
            else{
                argv.push_back(arg);
            }
        }

        int run(void) noexcept{
            if(argv.empty()){
                return -1;
            }
            if(!argv.front()){
                return -1;
            }

            if(argv.back() != nullptr){
                argv.push_back(nullptr);
            }

            return InrRun::execute_command(argv.size() - 1, argv.data());
        }

        void pop_arg(void) noexcept{
            argv.pop_back();
        }

        const std::vector<const char*>& get_args(void) const noexcept{
            return argv;
        }
    };
}

#endif // INERTIA_PROCESS_HPP
