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

#include "../Target/x86/x86Triple.h"

namespace inr {

const char* Triple::getTIRAsmStr(Arch arch) noexcept {
    switch(arch) {
        case Arch::Unknown:
            return nullptr;
        case Arch::x86_64:
            return x86::getTIRAsmStr();
    }
}

const TIRTargetDesc* Triple::getTIRTargetDesc(Arch arch) noexcept {
    switch(arch) {
        case Arch::Unknown:
            return nullptr;
        case Arch::x86_64:
            return x86::getTIRTargetDesc();
    }
}

Triple::TIRMatcherFunc Triple::getTIRMatchingFunc(Arch arch) noexcept {
    switch(arch) {
        case Arch::Unknown:
            return nullptr;
        case Arch::x86_64:
            return x86::getTIRMatchingFunc();
    }
}

Triple::FileType Triple::getFileType(OS os) noexcept {
    switch(os) {
        case OS::Unknown:
        case OS::Linux:
            return FileType::ELF;
    }
}

const RegisterInfo* Triple::getRegisterInfo(Arch arch) noexcept {
    switch(arch) {
        case Arch::Unknown:
            return nullptr;
        case Arch::x86_64:
            return x86::getRegisterInfo();
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
                         CallingConv& cc) noexcept {
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

static inline CCFunc getFinalCC(Triple::Arch arch, Triple::OS os,
                                Triple::ABI abi, CallingConv cc,
                                bool ret) noexcept {
    if(getCC(arch, os, abi, cc)) return nullptr;
    switch(arch) {
        case Triple::Arch::x86_64:
            return x86::getCCFunc(cc, ret);
        case Triple::Arch::Unknown:
            inr_notpossible("Unknown architecture isn't a thing.");
    }
}

CCFunc Triple::getCCArgs(Arch arch, OS os, ABI abi, CallingConv cc) noexcept {
    return getFinalCC(arch, os, abi, cc, false);
}

CCFunc Triple::getCCRet(Arch arch, OS os, ABI abi, CallingConv cc) noexcept {
    return getFinalCC(arch, os, abi, cc, true);
}

arrview<Register> Triple::getCalleeSaved(Arch arch, CallingConv cc) noexcept {
    if(cc == CallingConv::C) return {};

    switch(arch) {
        case Arch::Unknown:
            return {};
        case Arch::x86_64:
            return x86::getCalleeSaved(cc);
    }
}

arrview<Register> Triple::getCallerSaved(Arch arch, CallingConv cc) noexcept {
    if(cc == CallingConv::C) return {};

    switch(arch) {
        case Arch::Unknown:
            return {};
        case Arch::x86_64:
            return x86::getCallerSaved(cc);
    }
}

CallingConv Triple::getDefaultCC(Triple triple) noexcept {
    CallingConv cc = CallingConv::C;
    getCC(triple.getArch(), triple.getOS(), triple.getABI(), cc);
    return cc;
}

unsigned Triple::getCallAlignment() const noexcept {
    switch(arch_) {
        case Arch::Unknown:
        case Arch::x86_64:
            return 16;
    }
}

unsigned Triple::getFunctionEntryStackSize() const noexcept {
    switch(arch_) {
        case Arch::Unknown:
        case Arch::x86_64:
            return 8;
    }
}

} // namespace inr