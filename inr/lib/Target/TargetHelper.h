// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
//
// THIS FILE SHOULD NOT BE IN `include` AND SHOULD BE ONLY USED BY TARGETS.
#ifndef INERTIA_TARGET_TARGETHELPER_H
#define INERTIA_TARGET_TARGETHELPER_H

#include <inr/TIR/TIRInstruction.h>

namespace inr {
class TIRInstInfoTableHelper {
    constexpr static unsigned TABLE_SIZE =
        (unsigned)TIRInstID::TIR_INST_END - (unsigned)TIRInstID::TIR_INST_START;
    TIRInstInfo info_[TABLE_SIZE];

public:
    constexpr TIRInstInfoTableHelper() noexcept : info_() {
        for(unsigned i = 0; i < TABLE_SIZE; i++) {
            info_[i] = TIRInstInfo();
        }
    }

    constexpr TIRInstInfoTableHelper(
        std::initializer_list<std::pair<TIRInstID, TIRInstInfo>>
            exceptions) noexcept :
        TIRInstInfoTableHelper() {
        for(auto it = exceptions.begin(); it != exceptions.end(); ++it) {
            info_[(unsigned)it->first] = it->second;
        }
        info_[(unsigned)TIRInstID::STORE] =
            TIRInstInfo({}, {TIRInstInfo::OperandAllowed::RegMem,
                             TIRInstInfo::OperandAllowed::RegMemImm});
    }

    constexpr arrview<TIRInstInfo> getInfo() const noexcept {
        return arrview<TIRInstInfo>(info_, TABLE_SIZE);
    }
};
} // namespace inr

#endif // INERTIA_TARGET_TARGETHELPER_H
