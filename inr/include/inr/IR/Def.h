// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_DEF_H
#define INERTIA_IR_DEF_H

/// @file IR/Def.h
/// @brief Represents a definition.

#include <inr/ADT/ArrView.h>
#include <inr/ADT/IVector.h>
#include <inr/IR/Type.h>
#include <inr/Support/Assert.h>

#include <string_view>

namespace inr {

/// @brief Base class for all defs.
/// @note This class' equivalent is LLVM's `Value` class.
///
/// The `Def` class represents a definition, and something that can be on the
/// rhs. For example an instruction, function, basic block, etc.. Those are all
/// definition, as all of the above can appear on the right hand side, have
/// users, and are declared.
class Def {
public:
    enum DefType {
        ArgDefType,
        BlockDefType,
        FuncDefType,
        InstDefType,
        ConstDefType,
        UnDefDefType,
    };

private:
    const Type* type_;
    DefType defType_;
    std::string_view name_;
    ivec<Def*, 4> users_;

public:
    /// @brief Default constructor for a def.
    Def(const Type* type, DefType defType, std::string_view name = {}) :
        type_(type), defType_(defType), name_(name) {
        inr_assert(type != nullptr, "Def Def(): passed in nullptr for type");
    }

    Def(const Def&) = delete;
    Def& operator=(const Def&) = delete;

    Def(Def&&) noexcept = default;
    Def& operator=(Def&&) noexcept = default;

    virtual ~Def() = default;

    /// @brief Returns this def's name.
    std::string_view getName() const {
        return name_;
    }

    /// @brief Updates this def's name.
    void setName(std::string_view name) {
        name_ = name;
    }

    /// @brief Returns the type of this def.
    const Type* getType() const {
        return type_;
    }

    /// @brief Adds a user of this def.
    void addUser(Def* def) {
        inr_assert(def != nullptr, "Def addUser(): passed in a nullptr def");
        users_.emplace_back(def);
    }

    /// @brief Removes a user of this def.
    void removeUser(Def* def) {
        inr_assert(def != nullptr, "Def removeUser(): passed in a nullptr def");
        users_.erase_if_found(def);
    }

    /// @brief Returns the def's users.
    arrview<Def*> getUsers() const {
        return users_;
    }

    /// @brief Equivalent of doing `getUsers().size() != 0`.
    bool hasUsers() const {
        return users_.size() != 0;
    }

    DefType getDefType() const {
        return defType_;
    }
};

} // namespace inr

#endif // INERTIA_IR_DEF_H
