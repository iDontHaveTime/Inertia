#ifndef ASSEMBLY_HPP
#define ASSEMBLY_HPP

#include <string>

namespace Inertia{
    struct AssemblyOperand{
        const AssemblyOperand* offsetBy = nullptr;
        enum OperandType {UNKNOWN, REGISTER, LABEL, MEMORY, DIRECT} type = UNKNOWN;
        std::string_view name;
        std::string_view extra;

        AssemblyOperand() = default;
        AssemblyOperand(OperandType t, const std::string_view& n, const std::string_view& e = "") : type(t), name(n), extra(e){};
        AssemblyOperand(OperandType t, const std::string_view& n, const AssemblyOperand* offset, const std::string_view& e = "") : offsetBy(offset), type(t), name(n), extra(e){};

        AssemblyOperand(OperandType t, const char* n, const std::string_view& e = "") : type(t), name(n), extra(e){};
        AssemblyOperand(OperandType t, const char* n, const AssemblyOperand* offset, const std::string_view& e = "") : offsetBy(offset), type(t), name(n), extra(e){};
    };
    struct AssemblyMnemonic{
        std::string_view name;

        AssemblyMnemonic() = default;
        AssemblyMnemonic(const std::string_view& n) : name(n){};
        AssemblyMnemonic(const char* n) : name(n){};
    };
    struct AssemblyComment{
        std::string comment;

        AssemblyComment() = default;
        AssemblyComment(const std::string& com) : comment(com){};
        AssemblyComment(const char* str) : comment(str){};
    };
    struct AssemblyInstruction{
        AssemblyMnemonic mnemonic;
        AssemblyOperand dest;
        AssemblyOperand src1, src2, src3;

        AssemblyInstruction() = default;
        AssemblyInstruction(const AssemblyMnemonic& _mnemonic) : mnemonic(_mnemonic){};
        AssemblyInstruction(const AssemblyMnemonic& _mnemonic, const AssemblyOperand& _dest) : mnemonic(_mnemonic), dest(_dest){};
        AssemblyInstruction(const AssemblyMnemonic& _mnemonic, const AssemblyOperand& _dest, const AssemblyOperand& op1) : mnemonic(_mnemonic), dest(_dest), src1(op1){};
        AssemblyInstruction(const AssemblyMnemonic& _mnemonic, const AssemblyOperand& _dest, const AssemblyOperand& op1, const AssemblyOperand& op2) : mnemonic(_mnemonic), dest(_dest), src1(op1), src2(op2){};
        AssemblyInstruction(const AssemblyMnemonic& _mnemonic, const AssemblyOperand& _dest, const AssemblyOperand& op1, const AssemblyOperand& op2, const AssemblyOperand& op3) : mnemonic(_mnemonic), dest(_dest), src1(op1), src2(op2), src3(op3){};
    };
    struct AssemblyLabel{
        std::string name;

        AssemblyLabel() = default;
        AssemblyLabel(const std::string& _n) : name(_n){};
    };

    struct AssemblyVariant{
        AssemblyInstruction ins;
        AssemblyLabel lbl;
        AssemblyComment comment;
        enum ASMV {NONE, INSTRUCTION, LABEL} type = NONE;

        AssemblyVariant() = default;
        AssemblyVariant(const AssemblyLabel& label, const AssemblyComment& c = "") : lbl(label), comment(c), type(LABEL){};
        AssemblyVariant(const AssemblyInstruction& instruction, const AssemblyComment& c = "") : ins(instruction), comment(c), type(INSTRUCTION){};
    };
}

#endif // ASSEMBLY_HPP
