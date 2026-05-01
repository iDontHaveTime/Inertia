// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/Target/Triple.h>
#include <inrcc/Options/Data.h>

namespace inrcc {

CData::CData(inr::Triple triple) {
    ptrW_ = triple.getPointerWidth();
    switch(triple.getArch()) {
        case inr::Triple::Arch::x86_64:
            // These are the usual on x86-64
            charW_ = 8;
            shortW_ = 16;
            intW_ = 32;
            sizeW_ = 64;
            llW_ = 64;

            switch(triple.getOS()) {
                case inr::Triple::OS::Unknown:
                    [[fallthrough]];
                case inr::Triple::OS::Linux:
                    longW_ = 64;
                    break;
            }

            break;
        case inr::Triple::Arch::Unknown:
            return;
    }
}

} // namespace inrcc