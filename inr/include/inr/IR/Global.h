// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_GLOBAL_H
#define INERTIA_IR_GLOBAL_H

/// @file IR/Global.h
/// @brief Provides a global value.

#include <inr/IR/Value.h>
#include <inr/Support/Align.h>

namespace inr {

class Global : public Value {
public:
    enum Linkage {
        External, ///< External linkage, symbol is visible.
        Internal, ///< Symbol is NOT exported.
        Weak,     ///< Symbol may be overriden.
        Common    ///< May be merged.
    };

private:
    Linkage linkage_;
    Alignment alignment_;

public:
    Global(Linkage linkage, Alignment alignment, ValueID id, const Type* type,
           sview name = {}) noexcept :
        Value(id, type, name), linkage_(linkage), alignment_(alignment) {}

    Linkage getLinkage() const noexcept {
        return linkage_;
    }

    void setLinkage(Linkage linkage) noexcept {
        linkage_ = linkage;
    }

    Alignment getAlignment() const noexcept {
        return alignment_;
    }

    void setAlignment(Alignment alignment) noexcept {
        alignment_ = alignment;
    }
};

} // namespace inr

#endif // INERTIA_IR_GLOBAL_H
