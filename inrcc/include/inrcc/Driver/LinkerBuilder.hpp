#ifndef INRCC_LINKERBUILDER_HPP
#define INRCC_LINKERBUILDER_HPP

/* BUILDS THE FINAL LINKER COMMAND */
#include "inrcc/Driver/Linker.hpp"

namespace inrcc{
    class LinkerBuilder{
        Linker* linker;

    public:

        constexpr void setLinker(Linker* _linker) noexcept{
            linker = _linker;
        }

        constexpr Linker* getLinker(void) const noexcept{
            return linker;
        }

        constexpr LinkerBuilder() noexcept = default;
        constexpr LinkerBuilder(Linker* _linker) noexcept : linker(_linker){};
    };
}

#endif // INRCC_LINKERBUILDER_HPP
