#ifndef INERTIA_MCELFWRITER_HPP
#define INERTIA_MCELFWRITER_HPP

/**
 * @file inr/MC/MCELFWriter.hpp
 * @brief ELF Writer derived from MCWriter.
 *
 *
 **/

#include "inr/MC/MCOBJ.hpp"
#include "inr/MC/MCWriter.hpp"
#include "inr/Support/Binary.hpp"
#include "inr/Support/Stream.hpp"

namespace inr{

    class MCELFWriter : public MCWriter{
        elf_header* hdr;

        void new_elf();
        void end_elf();
    public:
        MCELFWriter(MCOBJ& _obj, inr_ostream& os) noexcept : MCWriter(_obj, os){
            new_elf();
        }

        bool valid() const noexcept override{
            return hdr != nullptr;
        }

        size_t write() override;

        ~MCELFWriter() noexcept override{
            end_elf();
        }
    };

}

#endif // INERTIA_MCELFWRITER_HPP
