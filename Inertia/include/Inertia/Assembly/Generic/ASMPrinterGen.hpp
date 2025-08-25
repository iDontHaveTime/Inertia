#ifndef INERTIA_ASMPRINTERGEN_HPP
#define INERTIA_ASMPRINTERGEN_HPP

#include "Inertia/Lowering/LoweredOut.hpp"
#include <ostream>

namespace Inertia{
    class ASMPrinterGeneric{
    public:

        ASMPrinterGeneric() noexcept = default;

        virtual ~ASMPrinterGeneric() noexcept = default;

        virtual bool output(const LoweredOutput& lowout, std::ostream& os);

        friend class ASMPrinter;
    };
}

#endif // INERTIA_ASMPRINTERGEN_HPP
