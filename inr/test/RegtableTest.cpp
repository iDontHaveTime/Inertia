// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/ADT/ArrView.h>
#include <inr/ADT/StrView.h>
#include <inr/MIR/Register.h>
#define x86_REGISTER_HEADER
#include <inr/Target/x86/x86Registers.inc>

int main() {
    constexpr const inr::RegisterInfo& x86Info = inr::x86::RegInfo;

    constexpr inr::sview rax = x86Info.getName(x86Info.getByName("rax"));

    inr::outs() << "Found register: " << rax << '\n';

    inr::outs() << "Found regclass: "
                << x86Info.getName(x86Info.getRegClass("GR64")) << '\n';

    inr::arrview<inr::Register> subregsRax = x86Info.getSubRegs(inr::x86::RAX);

    for(inr::Register reg : subregsRax) {
        inr::outs() << x86Info.getName(reg) << '\n';
    }

    inr::arrview<inr::Register> GPR64 = x86Info.getRegClassRegs(inr::x86::GR64);
    for(inr::Register reg : GPR64) {
        inr::outs() << x86Info.getName(reg) << '\n';
    }

    return 0;
}