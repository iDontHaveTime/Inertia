// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_USEDEF_H
#define INERTIA_IR_USEDEF_H

/// @file IR/UseDef.h
/// @brief A definition that uses other ones.

#include <inr/ADT/ArrView.h>
#include <inr/ADT/IVector.h>
#include <inr/IR/Def.h>
#include <inr/IR/Type.h>
#include <inr/Support/Assert.h>

namespace inr {

/// @brief A def that tracks its uses.
class UseDef : public Def {
    ivec<Def*, 4> uses_;

public:
    /// @brief Default constructor for a UseDef.
    UseDef(const Type* type, DefType defType, std::string_view name = {}) :
        Def(type, defType, name) {}

    /// @brief Adds the def to uses, and adds itself to its users.
    void addUse(Def* def) {
        inr_assert(def != nullptr, "UseDef addUse(): passed in a nullptr def");
        uses_.emplace_back(def);
        def->addUser(this);
    }

    /// @brief Removes the uses, and removes itself from the users.
    void removeUses() {
        for(auto it = uses_.rbegin(); it != uses_.rend(); ++it) {
            (*it)->removeUser(this);
        }
    }

    arrview<Def*> getUses() const {
        return uses_;
    }
};

} // namespace inr

#endif // INERTIA_IR_USEDEF_H
