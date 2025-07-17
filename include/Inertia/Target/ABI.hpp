#ifndef ABI_HPP
#define ABI_HPP

#include "Inertia/Target/Register.hpp"
#include <cstdint>
#include <vector>

/* THE BACKEND CODEGEN MUST COMPLY WITH THESE, ALL OF THESE! */
/* MUST NOT OWN POINTERS */

namespace Inertia{
    typedef uint16_t alignment;
    typedef uint16_t asmsize;

    class alignas(32) ABI{
    public:
        Register* GeneralReturn = nullptr;
        Register* FloatReturn = nullptr;

        std::vector<Register*> GeneralArgs;
        std::vector<Register*> FloatArgs;

        std::vector<Register*> GeneralCalleeSaved;
        std::vector<Register*> GeneralCallerSaved;

        std::vector<Register*> FloatCalleeSaved;
        std::vector<Register*> FloatCallerSaved;

        /* IF ABOVE GENERAL RETURN SIZE AND BELOW HIDDEN POINTER SIZE USE THESE! */
        std::vector<Register*> ExtraReturnRegistersGeneral;
        /* SAME HERE BUT FLOAT, NOT USED MOST OF THE TIME*/
        std::vector<Register*> ExtraReturnRegistersFloat;
        /* THESE ARE IN ORDER AND SIZE MATTERS */

        bool UseChildrenForReturnRemainder = false;
        /*
            FOR EXAMPLE IF STRUCT IS 12 BYTES AND USES RAX AND RBX WILL USE RAX AND EBX IF TRUE, 
            IF FALSE WILL USE RAX AND RBX.
            REGISTERS MUST HAVE A CHILD NODE FOR THIS TO BE ACTIVE! 
        */

        alignment FunctionAlign = 8;
        alignment StackAlign = 8;

        asmsize ShadowSpace = 0;
        asmsize RedZone = 0;

        asmsize PassInHiddenPointerAfterSize = 16;
        asmsize ReturnHiddenPointerAfterSize = 16;

        asmsize StackPrologueReserve = 0;

        Register* FrameRegister = nullptr;
        Register* StackRegister = nullptr;

        asmsize MinimumReturnSize = 0;
        bool SignExtendReturn = true;
        /* IF RETURN BELOW MINIMUM SIZE, SIGN EXTEND IT */

        // IF MIXED USES ONLY THE GeneralArgs.size();
        bool MixedArguments = false;
        /*
        EXPLANATION:
        Non Mixed (SystemV):
        RDI RSI RDX...
        XMM0 XMM1...
        Mixed (Windows):
        RCX XMM1 R8 XMM2...
        */

        // STACK ARGUMENTS
        bool ReversePushOrder = false;
        /*
        int foo(int x, int y, int z)
        FALSE:
        push z, y, x
        TRUE:
        push x, y, z
        */

        bool ShadowSpaceZero = false; // Zeroes out shadow space

        /* 
        IF PASSED IN A 4 BYTE ARG INTO 8 BYTE REGISTER, 
        IF FALSE MAKE IT 4 BYTE REGISTER, 
        IF TRUE ZERO EXTEND.
        */
        bool PromoteGeneralArgs = false;
        bool PromoteFloatArgs = false; 

        // set to null if not used
        // Puts all the amount of that type of arguments into that register
        Register* PutAmountOfGeneralPurposeArgs = nullptr; 
        Register* PutAmountOfFloatArgs = nullptr;

        // Puts the amount of that type that is put into registers
        Register* PutAmountOfGeneralPurposeArgsInRegisters = nullptr; 
        Register* PutAmountOfFloatArgsInRegisters = nullptr;

        // Puts the amount of that type that is put into stack
        Register* PutAmountOfGeneralPurposeArgsInStack = nullptr; 
        Register* PutAmountOfFloatArgsInStack = nullptr;



        ABI() = default;
    };
}

#endif // ABI_HPP
