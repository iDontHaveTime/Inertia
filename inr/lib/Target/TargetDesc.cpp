// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/Target/Target.h>
#include <inr/Target/TargetDesc.h>

#include <initializer_list>
#include <vector>

#define ADD_TARGET_ENTRY(NAME)                         \
    namespace inr {                                    \
    extern const TargetRegistry* Target##NAME##Init(); \
    }
#include "TargetEntries.inc"
#undef ADD_TARGET_ENTRY

namespace inr {

class TargetDescRegistry {
    std::vector<const TargetRegistry*> desc_;

public:
    TargetDescRegistry(std::initializer_list<const TargetRegistry*> entries) {
        desc_.insert(desc_.end(), entries.begin(), entries.end());
    }

    std::unique_ptr<TargetInfo> findTarget(Target target) {
        for(auto td : desc_) {
            if(td->matches(target)) {
                return td->newInfo(target);
            }
        }
        return {};
    }
};

std::unique_ptr<TargetInfo> TargetRegistry::getDesc(Target target) {
    static TargetDescRegistry registery{
#define ADD_TARGET_ENTRY(NAME) Target##NAME##Init(),
#include "TargetEntries.inc"
#undef ADD_TARGET_ENTRY
    };
    return registery.findTarget(target);
}

} // namespace inr
