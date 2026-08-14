// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TARGET_TARGETDESC
#define INERTIA_TARGET_TARGETDESC

#include <inr/IR/CallingConv.h>
#include <inr/Target/Mangling.h>
#include <inr/Target/Register.h>
#include <inr/Target/Target.h>

#include <memory>

namespace inr {

class TIRCCLowering;

struct TargetFeatures {
    virtual ~TargetFeatures() = default;
};

class TargetInfo {
public:
    using TIRCCLoweringGetter = const TIRCCLowering* (*)(CallingConv);

private:
    unsigned ptrWidth_;
    Mangling mangling_;
    bool bigEndian_;
    CallingConv defCC_;
    const TargetRegisters* registers_;
    TIRCCLoweringGetter cclowering_;
    std::unique_ptr<TargetFeatures> features_;

public:
    TargetInfo(unsigned ptrWidth, Mangling mangling, bool be, CallingConv cc,
               const TargetRegisters* regs, TIRCCLoweringGetter cclowering,
               TargetFeatures* tf) :
        ptrWidth_(ptrWidth),
        mangling_(mangling),
        bigEndian_(be),
        defCC_(cc),
        registers_(regs),
        cclowering_(cclowering),
        features_(tf) {}

    unsigned getPtrWidth() const {
        return ptrWidth_;
    }

    Mangling getMangling() const {
        return mangling_;
    }

    bool isLittleEndian() const {
        return !bigEndian_;
    }

    bool isBigEndian() const {
        return bigEndian_;
    }

    const TargetRegisters* getRegisters() const {
        return registers_;
    }

    CallingConv getDefaultCC() const {
        return defCC_;
    }

    const TIRCCLowering* getTIRLowering(CallingConv cc) const {
        if(cc == CallingConv::Default) cc = defCC_;
        return cclowering_(cc);
    }

    const TargetFeatures* getFeatures() const {
        return features_.get();
    }
};

class TargetRegistry {
    virtual std::unique_ptr<TargetInfo> newInfo(Target target) const = 0;
    virtual bool matches(Target target) const = 0;

    friend class TargetDescRegistry;

public:
    static std::unique_ptr<TargetInfo> getDesc(Target target);
};

} // namespace inr

#endif // INERTIA_TARGET_TARGETDESC
