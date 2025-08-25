#ifndef INERTIA_IRPRINTER_HPP
#define INERTIA_IRPRINTER_HPP

#include "Inertia/Assembly/Generic/ASMPrinterGen.hpp"
#include "Inertia/IR/Frame.hpp"
#include <filesystem>

namespace Inertia{
    class IRPrinter{
    public:

        IRPrinter() noexcept = default;

        bool output(const Frame& frame, std::ostream& os);

        ~IRPrinter() noexcept = default;
    };
}

#endif // INERTIA_IRPRINTER_HPP
