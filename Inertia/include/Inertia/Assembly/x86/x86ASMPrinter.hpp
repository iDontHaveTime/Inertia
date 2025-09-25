#ifndef INERTIA_X86ASMPRINTER_HPP
#define INERTIA_X86ASMPRINTER_HPP

#include "Inertia/Assembly/Generic/ASMPrinterGen.hpp"

/* This is the x86 Assembly printer, x86-64 to be exact. */

namespace Inertia{
    class x86ASMPrinter : public ASMPrinterGeneric{

    public:

        /* As said by every description, outputs assembly, specifically x86 from the lowered output. */
        bool output(const LoweredOutput& lowout, std::ostream& os) override;
        x86ASMPrinter() noexcept = default;
    };
}

#endif // INERTIA_X86ASMPRINTER_HPP
