#ifndef INERTIA_TARGETOUTPUT_HPP
#define INERTIA_TARGETOUTPUT_HPP

#include "Inertia/Mem/Archmem.hpp"
#include "Inertia/Target/TargetFile.hpp"
#include <cstdint>
#include <string_view>
#include <vector>

namespace Inertia{
    enum class TargetParserType{
        REGCLASS,
        REGISTER,
        DATAENT,
        DATAFIELD,
        INSTRUCTION,
        EXTENSION
    };
    enum class DataType{
        BIT,
    };
    struct DataInitEntry{
        bool init;
        uintmax_t val;
        size_t di;
    };
    struct Data{
        std::string_view name;
        DataType type = DataType::BIT;
        int width = 1;
        uintmax_t def_init = 0;
        bool had_default = false;
    };
    struct DataEntry{
        std::string_view name;
        std::vector<Data> data;
    };
    struct InstructionOperand{
        std::string_view name; // register/class name
        std::string_view extra_name; // operand name
        TargetParserType type;
    };
    struct InstructionFormatee{
        std::string_view name;
        enum class FormatField{
            NONE,
            NAME
        } field;

        InstructionFormatee() = default;
        InstructionFormatee(const std::string_view& str) noexcept : name(str){};
    };
    struct InstructionFormat{
        std::string_view fmt;
        std::vector<InstructionFormatee> formatees;
    };
    struct InstructionEntry{
        std::string_view name;
        std::vector<InstructionOperand> ops;
        InstructionFormat fmt;

        InstructionEntry() = default;
        InstructionEntry(const std::string_view& view) noexcept : name(view), ops(){};
    };
    struct ExtensionEntry{
        std::string_view name;
    };
    struct RegisterEntry{
        std::string_view name;
        std::string_view classname;
        int width = 0;

        std::vector<std::string> init;
        std::vector<size_t> dataIndeces;
        std::vector<DataInitEntry> inits;

        std::string_view parent;
    };
    struct CPPInclude{
        std::string_view name;
        enum class CPPIncludeType{
            Arrows, Quotes
        } type;
        CPPInclude() = default;
        CPPInclude(const std::string_view& str, CPPIncludeType t) : name(str), type(t){};
    };
    struct TargetOutput{
        const TargetFile file;

        std::string target;
        Endian endian = Endian::IN_UNKNOWN_ENDIAN;

        std::vector<std::string_view> regclasses;
        std::vector<RegisterEntry> registers;
        std::vector<DataEntry> datas;
        std::vector<CPPInclude> cppinc;
        std::vector<InstructionEntry> instructions;
        std::vector<ExtensionEntry> extensions;

        TargetOutput(const TargetFile& lfile) noexcept : file(lfile){};
    };
}

#endif // INERTIA_TARGETOUTPUT_HPP
