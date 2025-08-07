#ifndef INERTIA_TARGETOUTPUT_HPP
#define INERTIA_TARGETOUTPUT_HPP

#include "Inertia/Mem/Archmem.hpp"
#include "Inertia/Target/TargetFile.hpp"
#include <string_view>
#include <vector>

namespace Inertia{
    struct RegisterEntry{
        std::string name;
        int classid = 0;
        int width = 0;

        std::vector<std::string> init;

        std::string_view parent;
    };
    struct CPPInclude{
        std::string name;
        enum class CPPIncludeType{
            Arrows, Quotes
        } type;
        CPPInclude() = default;
        CPPInclude(const std::string& str, CPPIncludeType t) : name(str), type(t){};
    };
    struct TargetOutput{
        const TargetFile file;

        std::string target;
        Endian endian = Endian::IN_UNKNOWN_ENDIAN;

        std::vector<std::string_view> regclasses;
        std::vector<RegisterEntry> registers;
        std::vector<CPPInclude> cppinc;

        TargetOutput(const TargetFile& lfile) noexcept : file(lfile){};
    };
}

#endif // INERTIA_TARGETOUTPUT_HPP
