// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/IR/CallingConv.h>
#include <inr/Target/Mangling.h>
#include <inr/Target/Target.h>
#include <inr/Target/TargetDesc.h>

#include "x86Defs.h"

namespace inr::x86 {

const TIRCCLowering* x86_32_ccget(CallingConv) {
    return nullptr;
}

const TIRCCLowering* x86_64_ccget(CallingConv cc) {
    switch(cc) {
        case CallingConv::Default:
            break;
        case CallingConv::SystemV:
            return getAMD64SysVCC();
    }
    return nullptr;
}

} // namespace inr::x86

namespace inr {

class x86TargetRegistry : public TargetRegistry {
    std::unique_ptr<TargetInfo> newInfo(Target target) const override {
        unsigned ptrWidth = 32;
        Mangling mangling = Mangling::None;
        CallingConv cc = CallingConv::SystemV;
        TargetInfo::TIRCCLoweringGetter cclowering = x86::x86_32_ccget;
        x86::CustomTargetFeatures* feats = new x86::CustomTargetFeatures();

        if(target.getArch() == Arch::x86_64) {
            ptrWidth = 64;
            cclowering = x86::x86_64_ccget;
            feats->long_mode = true;
            feats->sse = true;
            feats->sse2 = true;
        }

        if(target.getOS() == OS::Linux) {
            mangling = Mangling::ELF;
        }

        return std::make_unique<TargetInfo>(ptrWidth, mangling, false, cc,
                                            &x86::x86_registers, cclowering,
                                            feats);
    }
};

class x86_32TargetRegistry : public x86TargetRegistry {
    bool matches(Target target) const override {
        return target.getArch() == Arch::x86_32;
    }
};

class x86_64TargetRegistry : public x86TargetRegistry {
    bool matches(Target target) const override {
        return target.getArch() == Arch::x86_64;
    }
};

const TargetRegistry* Targetx86_64Init() {
    static x86_64TargetRegistry x86_64_registry;
    return &x86_64_registry;
}

const TargetRegistry* Targetx86_32Init() {
    static x86_32TargetRegistry x86_32_registry;
    return &x86_32_registry;
}

} // namespace inr
