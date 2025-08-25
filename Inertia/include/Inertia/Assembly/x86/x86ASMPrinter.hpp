#ifndef INERTIA_X86ASMPRINTER_HPP
#define INERTIA_X86ASMPRINTER_HPP

#include "Inertia/Assembly/Generic/ASMPrinterGen.hpp"

namespace Inertia{
    class x86ASMPrinter : public ASMPrinterGeneric{

    public:

        bool output(const LoweredOutput& lowout, std::ostream& os) override;
        x86ASMPrinter() noexcept = default;
    };
}

#endif // INERTIA_X86ASMPRINTER_HPP
