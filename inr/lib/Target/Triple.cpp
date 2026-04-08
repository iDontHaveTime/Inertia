// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/Support/Stream.h>
#include <inr/Target/Triple.h>

#include <bit>
#include <utility>

namespace inr {

constexpr std::pair<sview, Triple::Arch> arch_str_table[] = {
    {"unknown", Triple::Arch::Unknown}, {"x86_64", Triple::Arch::x86_64}};

constexpr std::pair<sview, Triple::OS> os_str_table[] = {
    {"unknown", Triple::OS::Unknown}, {"linux", Triple::OS::Linux}};

constexpr std::pair<sview, Triple::ABI> abi_str_table[] = {
    {"unknown", Triple::ABI::Unknown}, {"gnu", Triple::ABI::GNU}};

Triple Triple::getDefaultTriple() noexcept {
    Triple triple;

#ifdef __x86_64__
    triple.arch_ = Arch::x86_64;
#endif

#ifdef __linux__
    triple.os_ = OS::Linux;
    triple.abi_ = ABI::GNU;
#endif

    return triple;
}

Triple Triple::fromString(sview s) noexcept {
    Arch arch = Arch::Unknown;
    OS os = OS::Unknown;
    ABI abi = ABI::Unknown;

    if(s.count('-') != 2) {
        return Triple(arch, os, abi);
    }

    size_t first_dash = s.find('-');
    size_t second_dash = s.find('-', first_dash + 1);

    sview arch_str = s.slice(0, first_dash);
    sview os_str = s.slice(first_dash + 1, second_dash);
    sview abi_str = s.slice(second_dash + 1, s.size());

    for(const auto& a : arch_str_table) {
        if(arch_str == a.first) {
            arch = a.second;
        }
    }

    for(const auto& a : os_str_table) {
        if(os_str == a.first) {
            os = a.second;
        }
    }

    for(const auto& a : abi_str_table) {
        if(abi_str == a.first) {
            abi = a.second;
        }
    }

    return Triple(arch, os, abi);
}

sview Triple::getArchStr(Arch arch) noexcept {
    return arch_str_table[uint16_t(arch)].first;
}

sview Triple::getOSStr(OS os) noexcept {
    return os_str_table[uint16_t(os)].first;
}

sview Triple::getABIStr(ABI abi) noexcept {
    return abi_str_table[uint16_t(abi)].first;
}

std::string Triple::str() const {
    std::string result;

    result += getArchStr(arch_);

    result += '-';

    result += getOSStr(os_);

    result += '-';

    result += getABIStr(abi_);

    return result;
}

raw_stream& operator<<(raw_stream& os, Triple T) {
    return os << Triple::getArchStr(T.getArch()) << '-'
              << Triple::getOSStr(T.getOS()) << '-'
              << Triple::getABIStr(T.getABI());
}

unsigned Triple::getPointerWidth() const noexcept {
    switch(arch_) {
        case Arch::Unknown:
            return 0;
        case Arch::x86_64:
            return 64;
    }
}
std::endian Triple::getEndian() const noexcept {
    switch(arch_) {
        case Arch::Unknown:
            return std::endian::native;
        case Arch::x86_64:
            return std::endian::little;
    }
}

} // namespace inr

#include <inr/TIR/TIRLowering.h>
#define x86_TIR_HEADER
#include <inr/Target/x86/x86TIR.inc>

// TIR
namespace inr {

class TIRInstInfoTableHelper {
    constexpr static unsigned TABLE_SIZE =
        (unsigned)TIRInstID::TIR_INST_END - (unsigned)TIRInstID::TIR_INST_START;
    TIRInstInfo info_[TABLE_SIZE];

public:
    constexpr TIRInstInfoTableHelper() noexcept : info_() {
        for(unsigned i = 0; i < TABLE_SIZE; i++) {
            info_[i] = TIRInstInfo(true);
        }
    }

    constexpr TIRInstInfoTableHelper(
        std::initializer_list<std::pair<TIRInstID, TIRInstInfo>>
            exceptions) noexcept :
        TIRInstInfoTableHelper() {
        for(auto it = exceptions.begin(); it != exceptions.end(); ++it) {
            info_[(unsigned)it->first] = it->second;
        }
    }

    constexpr arrview<TIRInstInfo> getInfo() const noexcept {
        return arrview<TIRInstInfo>(info_, TABLE_SIZE);
    }
};

namespace x86 {
constexpr unsigned x86_integer_words[4] = {8, 16, 32, 64};
constexpr TIRInstInfoTableHelper x86_inst_info_table{
    {TIRInstID::ADD_DEST_SRC_SRC, TIRInstInfo(false)}};
constexpr TIRTargetDesc x86TargetDesc(x86_integer_words,
                                      x86_inst_info_table.getInfo());

} // namespace x86

const char* Triple::getTIRAsmStr(Arch arch) noexcept {
    switch(arch) {
        case Arch::Unknown:
            return nullptr;
        case Arch::x86_64:
            return x86::x86_TIRAsmStr;
    }
}

const TIRTargetDesc* Triple::getTIRTargetDesc(Arch arch) noexcept {
    switch(arch) {
        case Arch::Unknown:
            return nullptr;
        case Arch::x86_64:
            return &x86::x86TargetDesc;
    }
}

Triple::TIRMatcherFunc Triple::getTIRMatchingFunc(Arch arch) noexcept {
    switch(arch) {
        case Arch::Unknown:
            return nullptr;
        case Arch::x86_64:
            return x86::x86TIRMatchEmit;
    }
}

} // namespace inr

// clang-format off
#include <inr/Target/x86/x86Registers.h>
#include <inr/Target/x86/x86CallingConv.h>
// clang-format on

namespace inr {

const RegisterInfo* Triple::getRegisterInfo(Arch arch) noexcept {
    switch(arch) {
        case Arch::Unknown:
            return nullptr;
        case Arch::x86_64:
            return &x86::RegInfo;
    }
}

static inline CallingConv getx86_64CC(Triple::OS os, Triple::ABI abi) noexcept {
    switch(os) {
        case Triple::OS::Unknown:
            switch(abi) {
                case Triple::ABI::Unknown: // x86_64-unknown-unknown
                case Triple::ABI::GNU:     // x86_64-unknown-gnu
                    return CallingConv::SysV;
            }
        case Triple::OS::Linux:
            switch(abi) {
                case Triple::ABI::Unknown: // x86_64-linux-unknown
                case Triple::ABI::GNU:     // x86_64-linux-gnu
                    return CallingConv::SysV;
            }
    }
}

static inline bool getCC(Triple::Arch arch, Triple::OS os, Triple::ABI abi,
                         CallingConv& cc) {
    if(cc == CallingConv::C) {
        switch(arch) {
            case Triple::Arch::x86_64:
                cc = getx86_64CC(os, abi);
                break;
            case Triple::Arch::Unknown:
                return true;
        }
    }

    if(cc == CallingConv::C) return true;

    return false;
}

static inline CCFunc getx86CCFunc(CallingConv cc, bool ret) {
    switch(cc) {
        case CallingConv::SysV:
            return ret ? x86::CCRetSysV : x86::CCSysV;
        case CallingConv::C:
            __builtin_unreachable();
    }
}

static inline CCFunc getFinalCC(Triple::Arch arch, Triple::OS os,
                                Triple::ABI abi, CallingConv cc, bool ret) {
    if(getCC(arch, os, abi, cc)) return nullptr;
    switch(arch) {
        case Triple::Arch::x86_64:
            return getx86CCFunc(cc, ret);
        case Triple::Arch::Unknown:
            __builtin_unreachable();
    }
}

CCFunc Triple::getCCArgs(Arch arch, OS os, ABI abi, CallingConv cc) noexcept {
    return getFinalCC(arch, os, abi, cc, false);
}

CCFunc Triple::getCCRet(Arch arch, OS os, ABI abi, CallingConv cc) noexcept {
    return getFinalCC(arch, os, abi, cc, true);
}

} // namespace inr