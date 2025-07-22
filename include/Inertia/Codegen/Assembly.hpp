#ifndef ASSEMBLY_HPP
#define ASSEMBLY_HPP

#include <string>

namespace Inertia{
    struct AssemblyOperand{
        const AssemblyOperand* offsetBy = nullptr;
        enum OperandType {UNKNOWN, REGISTER, LABEL, MEMORY, DIRECT} type = UNKNOWN;
        std::string name;

        AssemblyOperand() = default;
        AssemblyOperand(OperandType t, const std::string& n) : type(t), name(n){};
        AssemblyOperand(OperandType t, const std::string& n, const AssemblyOperand* offset) : offsetBy(offset), type(t), name(n){};
    };
    struct AssemblyMnemonic{
        std::string name;

        AssemblyMnemonic() = default;
        AssemblyMnemonic(const std::string& n) : name(n){};
    };
    struct AssemblyInstruction{
        AssemblyMnemonic mnemonic;
        AssemblyOperand src1, src2, src3;

        AssemblyInstruction() = default;
        AssemblyInstruction(const AssemblyMnemonic& _mnemonic) : mnemonic(_mnemonic){};
        AssemblyInstruction(const AssemblyMnemonic& _mnemonic, const AssemblyOperand& op1) : mnemonic(_mnemonic), src1(op1){};
        AssemblyInstruction(const AssemblyMnemonic& _mnemonic, const AssemblyOperand& op1, const AssemblyOperand& op2) : mnemonic(_mnemonic), src1(op1), src2(op2){};
        AssemblyInstruction(const AssemblyMnemonic& _mnemonic, const AssemblyOperand& op1, const AssemblyOperand& op2, const AssemblyOperand& op3) : mnemonic(_mnemonic), src1(op1), src2(op2), src3(op3){};
    };
}

#endif // ASSEMBLY_HPP
