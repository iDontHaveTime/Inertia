// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Target/Target.h>

#include <cstddef>
#include <string>
#include <string_view>

namespace inr {

std::string_view getArchStr(Arch arch) {
    switch(arch) {
        case Arch::Error:
            return "none";
        case Arch::x86_64:
            return "x86_64";
        case Arch::x86_32:
            return "x86_32";
    }
}

std::string_view getOSStr(OS os) {
    switch(os) {
        case OS::Unknown:
            return "unknown";
        case OS::Linux:
            return "linux";
    }
}

std::string_view getABIStr(ABI abi) {
    switch(abi) {
        case ABI::None:
            return "none";
        case ABI::GNU:
            return "gnu";
    }
}

Arch getStrArch(std::string_view str) {
    if(str == "x86_64") return Arch::x86_64;
    else if(str == "x86_32") return Arch::x86_32;

    return Arch::Error;
}

OS getStrOS(std::string_view str) {
    if(str == "linux") return OS::Linux;

    return OS::Unknown;
}

ABI getStrABI(std::string_view str) {
    if(str == "gnu") return ABI::GNU;

    return ABI::None;
}

Target Target::fromString(std::string_view str) {
    return Target(str);
}

Target::Target(std::string_view str) {
    std::size_t pos = str.find('-');
    if(pos == str.npos) return;
    std::string_view arch = str.substr(0, pos);
    std::size_t ospos = str.find('-', pos + 1);
    if(ospos == str.npos) return;
    std::string_view os(str.data() + pos + 1, str.data() + ospos);
    std::string_view abi = str.substr(ospos + 1);

    arch_ = getStrArch(arch);
    os_ = getStrOS(os);
    abi_ = getStrABI(abi);
}

std::string Target::str() {
    std::string s(getArchStr(arch_));
    s += '-';
    s += getOSStr(os_);
    s += '-';
    s += getABIStr(abi_);
    return s;
}

} // namespace inr
