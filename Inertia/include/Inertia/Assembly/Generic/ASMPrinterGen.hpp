#ifndef INERTIA_ASMPRINTERGEN_HPP
#define INERTIA_ASMPRINTERGEN_HPP

#include "Inertia/Lowering/LoweredOut.hpp"
#include <filesystem>

namespace Inertia{
    enum class PrintingType{
        FILEIO, MEMORYIO
    };
    class ASMPrinterGeneric{
    protected:
        std::filesystem::path out;
    public:

        ASMPrinterGeneric() noexcept = default;

        void set_path(const std::filesystem::path& output){
            out = output;
        }

        ASMPrinterGeneric(const std::filesystem::path& output){
            set_path(output);
        }

        virtual ~ASMPrinterGeneric() noexcept = default;

        virtual bool output(const LoweredOutput& lowout, PrintingType pt);

        friend class ASMPrinter;
    };
}

#endif // INERTIA_ASMPRINTERGEN_HPP
