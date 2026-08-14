// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_GLOBALDEF_H
#define INERTIA_IR_GLOBALDEF_H

/// @file IR/GlobalDef.h
/// @brief Represents a global definition.

#include <inr/IR/Linkage.h>
#include <inr/IR/UseDef.h>

namespace inr {

/// @brief Represents a def with a linkage type.
class GlobalDef : public UseDef {
    Linkage linkage_;

public:
    /// @brief Default constructor for a global def.
    GlobalDef(Linkage linkage, const Type* type, DefType defType,
              std::string_view name = {}) :
        UseDef(type, defType, name), linkage_(linkage) {}

    /// @brief Returns the def's linkage.
    Linkage getLinkage() const {
        return linkage_;
    }

    /// @brief Modifies the current linkage.
    void setLinkage(Linkage linkage) {
        linkage_ = linkage;
    }
};

} // namespace inr

#endif // INERTIA_IR_GLOBALDEF_H
