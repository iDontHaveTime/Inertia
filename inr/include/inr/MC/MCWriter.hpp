#ifndef INERTIA_MCWRITER_HPP
#define INERTIA_MCWRITER_HPP

/**
 * @file inr/MC/MCWriter.hpp
 * @brief Base class for object files writers.
 *
 *
 **/

#include "inr/MC/MCOBJ.hpp"
#include "inr/Target/Triple.hpp"
#include "inr/Support/Stream.hpp"
#include "inr/Support/Alloc.hpp"

namespace inr{
    /**
     * @brief This is the base class for object files writers (ELF, PE, etc..).
     */
    class MCWriter{
    protected:
        inr_ostream& os;
        MCOBJ& obj;
    public:
        MCWriter(MCOBJ& _obj, inr_ostream& ost) noexcept : os(ost), obj(_obj){};

        MCWriter(const MCWriter&) = delete;
        MCWriter& operator=(const MCWriter&) = delete;

        const Triple& get_triple() const noexcept{
            return obj.get_triple();
        }

        inr_ostream& get_stream() noexcept{
            return os;
        }

        const inr_ostream& get_stream() const noexcept{
            return os;
        }

        allocator* get_allocator() const noexcept{
            return obj.get_allocator();
        }

        virtual bool valid() const noexcept = 0;
        virtual size_t write() = 0;

        operator bool() const noexcept{
            return valid();
        }

        virtual ~MCWriter() noexcept = default;
    };

}

#endif // INERTIA_MCWRITER_HPP
