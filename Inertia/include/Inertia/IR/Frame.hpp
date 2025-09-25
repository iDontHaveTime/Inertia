#ifndef INERTIA_FRAME_HPP
#define INERTIA_FRAME_HPP

#include "Inertia/IR/Function.hpp"
#include "Inertia/Target/Triple.hpp"
#include <vector>

/* Frame Class.
 * The Frame class is the main class for storing the IR internally, 
 * it stores everything related to the module of the IR.
*/

namespace Inertia{
    /* The main class of the IR module. */
    struct Frame{
        /* Target Triple of the Frame. */
        TargetTriple* ttriple;
        /* Functions stored inside the IR module. */
        std::vector<Function> funcs; // TODO: Change to ArenaLList!

        /* Default frame constructor. */
        Frame() noexcept = default;
        /* Frame with the target triple constructor. */
        Frame(TargetTriple* ttrip) noexcept : ttriple(ttrip), funcs(){};
    };
}
    

#endif // INERTIA_FRAME_HPP
