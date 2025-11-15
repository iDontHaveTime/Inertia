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
#include "inr/Support/Alloc.hpp"
#include "inr/Support/Binary.hpp"
#include "inr/Support/Stream.hpp"

namespace inr{

    template<inertia_allocator _mcelf_alloc_ = allocator>
    class MCELFWriter : public MCWriter<_mcelf_alloc_>{
        elf_header* hdr;

        void new_elf(){
            Triple triple = MCELFWriter<_mcelf_alloc_>::get_triple();

            if(is_arch_64bit(triple.get_arch())){
                hdr = MCWriter<_mcelf_alloc_>::template alloc<elf_header_64>(
                    get_arch_endian(triple.get_arch()),
                    triple_to_elf_abi(triple),
                    elf_type::ELF_REL,
                    triple_to_elf_isa(triple)
                );
            }
            else{
                hdr = MCWriter<_mcelf_alloc_>::template alloc<elf_header_32>(
                    get_arch_endian(triple.get_arch()),
                    triple_to_elf_abi(triple),
                    elf_type::ELF_REL,
                    triple_to_elf_isa(triple)
                );
            }
        }
        void end_elf(){
            Triple triple = MCELFWriter<_mcelf_alloc_>::get_triple();

            if(is_arch_64bit(triple.get_arch())){
                MCWriter<_mcelf_alloc_>::free((elf_header_64*)hdr);
            }
            else{
                MCWriter<_mcelf_alloc_>::free((elf_header_32*)hdr);
            }
        }
    public:
        MCELFWriter(MCOBJ<_mcelf_alloc_>& _obj, inr_ostream& os) noexcept : MCWriter<_mcelf_alloc_>(_obj, os){
            new_elf();
        }

        bool valid() const noexcept override{
            return hdr != nullptr;
        }

        size_t write() override{
            if(!valid()) return 0;

            size_t pos = 0;

            Triple triple = MCWriter<_mcelf_alloc_>::get_triple();

            bool flip_endians = endian::native != get_arch_endian(triple.get_arch());
            size_t amount_to_write = 0;

            char data_to_write[std::max(sizeof(elf_header_64), sizeof(elf_header_32))];


            if(is_arch_64bit(triple.get_arch())){
                amount_to_write = sizeof(elf_header_64);
                
                memcpy(data_to_write, hdr, sizeof(elf_header_64));

                elf_header_64* elf = (elf_header_64*)data_to_write;
                if(flip_endians) elf->flip_fields();
            }
            else{
                amount_to_write = sizeof(elf_header_32);

                memcpy(data_to_write, hdr, sizeof(elf_header_32));

                elf_header_32* elf = (elf_header_32*)data_to_write;
                if(flip_endians) elf->flip_fields();
            }

            pos += MCWriter<_mcelf_alloc_>::os.write(data_to_write, 1, amount_to_write);

            return pos;
        }

        ~MCELFWriter() noexcept override{
            end_elf();
        }
    };

}

#endif // INERTIA_MCELFWRITER_HPP
