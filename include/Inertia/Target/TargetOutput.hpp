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

        std::string_view parent;
    };
    struct TargetOutput{
        const TargetFile file;

        std::string target;
        Endian endian = Endian::IN_UNKNOWN_ENDIAN;

        std::vector<std::string_view> regclasses;
        std::vector<RegisterEntry> registers;

        TargetOutput(const TargetFile& lfile) noexcept : file(lfile){};
    };
}

#endif // INERTIA_TARGETOUTPUT_HPP
