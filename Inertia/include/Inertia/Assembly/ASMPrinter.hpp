#ifndef INERTIA_ASMPRINTER_HPP
#define INERTIA_ASMPRINTER_HPP

#include "Inertia/Assembly/Generic/ASMPrinterGen.hpp"
#include "Inertia/Lowering/LoweredOut.hpp"
#include "Inertia/Target/Triple.hpp"
#include <filesystem>

namespace Inertia{
    class ASMPrinter{
        ASMPrinterGeneric* internal = nullptr;
        TargetType tt = TargetType::None;
    public:

        ASMPrinter() noexcept = default;

        ASMPrinter(TargetType _tt) noexcept : tt(_tt){
            load_target(_tt);
        }

        ASMPrinter(const TargetTriple& _tt) noexcept : tt(_tt.getLoadedType()){
            load_target(_tt.getLoadedType());
        }

        void load_target(TargetType _tt);

        inline void load_target(const TargetTriple& _tt){
            load_target(_tt.getLoadedType());
        }

        inline void close_target() noexcept{
            if(internal){
                delete internal;
                internal = nullptr;
            }
            tt = TargetType::None;
        }

        inline bool set_path(const std::filesystem::path& output){
            if(!internal) return true;
            internal->set_path(output);
            return false;
        }

        inline TargetType getLoadedType() const noexcept{
            return tt;
        }

        inline ASMPrinterGeneric* getRaw() noexcept{
            return internal;
        }

        inline bool output(const LoweredOutput& lowout, PrintingType pt = PrintingType::FILEIO){
            if(!internal) return true;
            return internal->output(lowout, pt);
        }

        ~ASMPrinter() noexcept{
            close_target();
        }
    };
}

#endif // INERTIA_ASMPRINTER_HPP
