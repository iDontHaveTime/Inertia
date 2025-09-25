#ifndef INERTIA_ASMPRINTER_HPP
#define INERTIA_ASMPRINTER_HPP

#include "Inertia/Assembly/Generic/ASMPrinterGen.hpp"
#include "Inertia/Lowering/LoweredOut.hpp"
#include "Inertia/Target/Triple.hpp"
#include <ostream>

/* ASM Printer class.
 * This class manages the assembly printers for different architectures.
 * Once you load the target triple, it auto allocates the correct ASMPrinter class for the architecture.
*/

namespace Inertia{
    class ASMPrinter{
        /* The current loaded assembly printer. */
        ASMPrinterGeneric* internal = nullptr;
        /* The target type that the loaded assembly printer is using. */
        TargetType tt = TargetType::None;
    public:

        /* Just a simple constructor. */
        ASMPrinter() noexcept = default;

        /* Loads the assembly printer from target type. */
        ASMPrinter(TargetType _tt) noexcept : tt(_tt){
            load_target(_tt);
        }

        /* Loads the assembly printer from the target triple (recommended). */
        ASMPrinter(const TargetTriple& _tt) noexcept : tt(_tt.getLoadedType()){
            load_target(_tt.getLoadedType());
        }

        /* This is the function to load the printer from the target type. */
        void load_target(TargetType _tt);

        /* Just gets the triple's target into the load_target() function. */
        inline void load_target(const TargetTriple& _tt){
            load_target(_tt.getLoadedType());
        }

        /* Closes and frees the assembly printer. */
        inline void close_target() noexcept{
            if(internal){
                delete internal;
                internal = nullptr;
            }
            tt = TargetType::None;
        }

        /* Returs the currently loaded type. */
        inline TargetType getLoadedType() const noexcept{
            return tt;
        }

        /* Gets the raw assembly printer pointer. */
        inline ASMPrinterGeneric* getRaw() noexcept{
            return internal;
        }

        /* Outputs assembly to the output stream provided from the lowered output. */
        inline bool output(const LoweredOutput& lowout, std::ostream& os){
            if(!internal) return true;
            return internal->output(lowout, os);
        }

        /* The destructor frees the currently loaded assembly printer. */
        ~ASMPrinter() noexcept{
            close_target();
        }
    };
}

#endif // INERTIA_ASMPRINTER_HPP
