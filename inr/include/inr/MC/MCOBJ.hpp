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
#include "inr/Support/StringRef.hpp"
#include "inr/Target/Triple.hpp"

namespace inr{

    /**
     * @brief Labels of a segment.
     */
    class MCOBJLabel{
        strref name;
    };

    /**
     * @brief Segment of an object file.
     */
    class MCOBJSegment{
        inr_vec<MCOBJLabel> labels;
    };

    /**
     * @brief Generic machine code object.
     */
    template<inertia_allocator _mcobj_alloc_ = allocator>
    class MCOBJ{
        inr_vec<MCOBJSegment, _mcobj_alloc_> segments;
        Triple triple;
    public:
        MCOBJ(const Triple& tp) noexcept : segments(), triple(tp){};

        const Triple& get_triple() const noexcept{
            return triple;
        }

        _mcobj_alloc_ get_allocator() const noexcept{
            return {};
        }
    };

}

#endif // INERTIA_MCOBJ_HPP
