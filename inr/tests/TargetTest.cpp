// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Support/Stream.h>
#include <inr/Target/Host.h>
#include <inr/Target/Mangling.h>
#include <inr/Target/Target.h>
#include <inr/Target/TargetDesc.h>

int main() {
    inr::Target tg = inr::host::getTarget();

    inr::out() << tg.str() << '\n';

#if defined(__x86_64__) && defined(__gnu_linux__)
    inr::Target x86_64_linux_gnu("x86_64-linux-gnu");
    auto info = inr::TargetRegistry::getDesc(tg);
    if(!info) {
        inr::out() << "x86 not included in the build\n";
        return 0;
    }

    if(x86_64_linux_gnu.getArch() != tg.getArch()) {
        inr::err() << "Arch does not match\n";
        return 1;
    }
    if(x86_64_linux_gnu.getOS() != tg.getOS()) {
        inr::err() << "OS does not match\n";
        return 1;
    }
    if(x86_64_linux_gnu.getABI() != tg.getABI()) {
        inr::err() << "ABI does not match\n";
        return 1;
    }
    if(!info->isLittleEndian()) return 1;
    if(info->getMangling() != inr::Mangling::ELF) return 1;
    if(info->getPtrWidth() != 64) return 1;
#endif

    return 0;
}
