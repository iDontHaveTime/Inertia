#ifndef INERTIA_MCOBJ_HPP
#define INERTIA_MCOBJ_HPP

/**
 * @file inr/MC/MCOBJ.hpp
 * @brief Contains a generic object file.
 *
 *
 **/

#include "inr/Support/Alloc.hpp"
#include "inr/Support/Vector.hpp"
#include "inr/Target/Triple.hpp"

namespace inr{

    /**
     * @brief Segment of an object file.
     */
    class MCOBJSegment{

    };

    /**
     * @brief Generic machine code object.
     */
    class MCOBJ{
        inr_vec<MCOBJSegment> segments;
        Triple triple;
    public:
        MCOBJ(const Triple& tp, allocator* _mem = nullptr) noexcept : segments(_mem), triple(tp){};

        const Triple& get_triple() const noexcept{
            return triple;
        }

        allocator* get_allocator() const noexcept{
            return segments.get_allocator();
        }
    };

}

#endif // INERTIA_MCOBJ_HPP
