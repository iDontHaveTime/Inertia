#ifndef INERTIA_IRPRINTER_HPP
#define INERTIA_IRPRINTER_HPP

#include "Inertia/IR/Frame.hpp"

namespace Inertia{
    class IRPrinter{
    public:

        IRPrinter() noexcept = default;

        bool output(const Frame& frame, std::ostream& os);

        ~IRPrinter() noexcept = default;
    };
}

#endif // INERTIA_IRPRINTER_HPP
