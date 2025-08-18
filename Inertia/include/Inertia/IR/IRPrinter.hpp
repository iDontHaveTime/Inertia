#ifndef INERTIA_IRPRINTER_HPP
#define INERTIA_IRPRINTER_HPP

#include "Inertia/Assembly/Generic/ASMPrinterGen.hpp"
#include "Inertia/IR/Frame.hpp"
#include <filesystem>

namespace Inertia{
    class IRPrinter{
        std::filesystem::path out;
    public:

        IRPrinter() noexcept = default;

        IRPrinter(const std::filesystem::path& output){
            set_path(output);
        }

        void set_path(const std::filesystem::path& output){
            out = output;
        }

        bool output(const Frame& frame, PrintingType pt = PrintingType::FILEIO);

        ~IRPrinter() noexcept = default;
    };
}

#endif // INERTIA_IRPRINTER_HPP
