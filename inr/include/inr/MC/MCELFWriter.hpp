#ifndef INERTIA_MCELFWRITER_HPP
#define INERTIA_MCELFWRITER_HPP

/**
 * @file inr/MC/MCELFWriter.hpp
 * @brief ELF Writer derived from MCWriter.
 *
 *
 **/

#include "inr/MC/MCWriter.hpp"

namespace inr{

    class MCELFWriter : public MCWriter{
    public:
        using MCWriter::MCWriter;
    };

}

#endif // INERTIA_MCELFWRITER_HPP
