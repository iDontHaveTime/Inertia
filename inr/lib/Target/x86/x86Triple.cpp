// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include "../x86/x86Triple.h"

#include <inr/TIR/TIRLowering.h>

#include "../TargetHelper.h"

#ifdef INERTIA_INCLUDE_X86

#define x86_TIR_HEADER
#include <inr/Target/x86/x86TIR.inc>

#define x86_REGISTER_HEADER
#include <inr/Target/x86/x86Registers.inc>

#define x86_CALLING_CONV_HEADER
#include <inr/Target/x86/x86CallingConv.inc>

#endif

namespace inr::x86 {

#ifdef INERTIA_INCLUDE_X86

constexpr unsigned x86_integer_words[4] = {8, 16, 32, 64};
constinit TIRInstInfoTableHelper x86_inst_info_table{
    {TIRInstID::ADD_DEST_SRC_SRC, TIRInstInfo(false)},
    {TIRInstID::SUB_DEST_SRC_SRC, TIRInstInfo(false)},
    {TIRInstID::ADD_DEST_SRC,
     TIRInstInfo({}, {TIRInstInfo::OperandAllowed::RegMem,
                      TIRInstInfo::OperandAllowed::RegMemImm})},
    {TIRInstID::SUB_DEST_SRC,
     TIRInstInfo({}, {TIRInstInfo::OperandAllowed::RegMem,
                      TIRInstInfo::OperandAllowed::RegMemImm})},
    {TIRInstID::STORE, TIRInstInfo({},
                                   {TIRInstInfo::OperandAllowed::RegMem,
                                    TIRInstInfo::OperandAllowed::RegMemImm},
                                   true)}};
constexpr TIRTargetDesc x86TargetDesc(x86_integer_words,
                                      x86_inst_info_table.getInfo(), RSP, RBP);

const char* getTIRAsmStr() {
    return x86_TIRAsmStr;
}

const TIRTargetDesc* getTIRTargetDesc() {
    return &x86TargetDesc;
}

Triple::TIRMatcherFunc getTIRMatchingFunc() {
    return x86TIRMatchEmit;
}

const RegisterInfo* getRegisterInfo() {
    return &RegInfo;
}

CCFunc getCCFunc(CallingConv cc, bool ret) {
    switch(cc) {
        case CallingConv::C:
        case CallingConv::SysV:
            return ret ? CCRetSysV : CCSysV;
    }
}

arrview<Register> getCalleeSaved(CallingConv cc) {
    switch(cc) {
        case CallingConv::C:
            inr_notpossible("Triple's function checks if its C calling conv.");
        case CallingConv::SysV:
            return CCSysVCallee;
    }
}

arrview<Register> getCallerSaved(CallingConv cc) {
    switch(cc) {
        case CallingConv::C:
            inr_notpossible("Triple's function checks if its C calling conv.");
        case CallingConv::SysV:
            return CCSysVCaller;
    }
}

#else

const char* getTIRAsmStr() {
    return nullptr;
}
const TIRTargetDesc* getTIRTargetDesc() {
    return nullptr;
}
Triple::TIRMatcherFunc getTIRMatchingFunc() {
    return nullptr;
}
const RegisterInfo* getRegisterInfo() {
    return nullptr;
}
CCFunc getCCFunc(CallingConv, bool) {
    return nullptr;
}
arrview<Register> getCalleeSaved(CallingConv) {
    return {};
}
arrview<Register> getCallerSaved(CallingConv) {
    return {};
}

#endif

} // namespace inr::x86