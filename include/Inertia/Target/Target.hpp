#ifndef INERTIA_TARGET_HPP
#define INERTIA_TARGET_HPP

namespace Inertia{
    struct TargetInfo{
        enum TargetType {x86_64} target;
        bool PIC;

        TargetInfo() = default;
        TargetInfo(TargetType type) noexcept : target(type){};
    };
}

#endif // INERTIA_TARGET_HPP
