#ifndef INERTIA_TARGET_HPP
#define INERTIA_TARGET_HPP

namespace Inertia{
    struct TargetInfo{
        enum TargetType {x86_64} target;
        enum SyntaxStyle {GNUAS} syntax;

        const char* execSection; // .text
        const char* resSection; // .bss
        const char* dataSection; // .data
        const char* rodataSection; // .rodata

        bool PIC;
        bool useFramePointer;
        bool debug;

        TargetInfo() = default;
        TargetInfo(TargetType type) noexcept : target(type){};
    };
}

#endif // INERTIA_TARGET_HPP
