#ifndef INERTIA_MCWRITER_HPP
#define INERTIA_MCWRITER_HPP

/**
 * @file inr/MC/MCWriter.hpp
 * @brief Base class for object files writers.
 *
 *
 **/

#include "inr/Support/Stream.hpp"
#include "inr/Target/Triple.hpp"

namespace inr{
    /**
     * @brief This is the base class for object files writers (ELF, PE, etc..).
     */
    class MCWriter{
    protected:
        Triple& triple;
        inr_ostream& os;
    public:
        MCWriter(Triple& _triple, inr_ostream& ost) noexcept : triple(_triple), os(ost){};

        MCWriter(const MCWriter&) = delete;
        MCWriter& operator=(const MCWriter&) = delete;

        Triple& get_triple() const noexcept{
            return triple;
        }

        inr_ostream& get_stream() noexcept{
            return os;
        }

        const inr_ostream& get_stream() const noexcept{
            return os;
        }

        virtual ~MCWriter() = default;
    };

}

#endif // INERTIA_MCWRITER_HPP
