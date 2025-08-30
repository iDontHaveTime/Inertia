#ifndef INERTIA_PASSMANAGER_HPP
#define INERTIA_PASSMANAGER_HPP

#include "Inertia/Mem/Arenalist.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include "Inertia/Pass/Pass.hpp"

namespace Inertia{
    class PassManager{
        ArenaLList<Pass> passes;
    public:

        ArenaLList<Pass>& pass_list(void){
            return passes;
        }

        void run() noexcept{
            for(ArenaReference<Pass>& pass : passes){
                pass->run();
            }
        }
    };
}

#endif // INERTIA_PASSMANAGER_HPP
