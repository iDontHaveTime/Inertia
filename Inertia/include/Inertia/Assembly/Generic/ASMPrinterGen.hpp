#ifndef INERTIA_ASMPRINTERGEN_HPP
#define INERTIA_ASMPRINTERGEN_HPP

#include "Inertia/Lowering/LoweredOut.hpp"
#include <ostream>

/* ASM Printer Generic class.
 * This is the base class for all assembly printers available.
 * Managed by the ASMPrinter class.
*/

namespace Inertia{
    class ASMPrinterGeneric{
    public:

        /* The basic constructor. */
        ASMPrinterGeneric() noexcept = default;

        /* Virtual destructor. */
        virtual ~ASMPrinterGeneric() noexcept = default;

        /* Actually make the lowered output into assembly. */
        virtual bool output(const LoweredOutput& lowout, std::ostream& os);

        /* So that the loader can see the insides. */
        friend class ASMPrinter;
    };
}

#endif // INERTIA_ASMPRINTERGEN_HPP
