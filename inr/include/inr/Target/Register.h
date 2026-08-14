// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TARGET_REGISTER
#define INERTIA_TARGET_REGISTER

/// @file Target/Register.h
/// @brief Defines a register class for targets.

#include <inr/ADT/ArrView.h>
#include <inr/Support/Assert.h>
#include <inr/TIR/TIRT.h>

#include <algorithm>
#include <cstdint>
#include <string_view>

namespace inr {

struct Register {
    std::string_view name;
    uint32_t id; ///< Register ID should match its index in the
                 ///< `TargetRegisters` array.

    bool operator==(const Register& other) const {
        return id == other.id;
    }
};

struct RegisterFamily {
    uint32_t id;
    arrview<uint32_t> registers;
};

struct RegisterClass {
    uint32_t id;
    arrview<uint32_t> registers;
    TIRT type;
};

struct TargetRegisters {
    arrview<Register> registers;
    arrview<RegisterFamily> families;
    arrview<RegisterClass> classes;

    /// @brief Returns whether or not register is in that family.
    bool inFamily(uint32_t familyID, uint32_t registerID) const {
        inr_assert(familyID < families.size(),
                   "TargetRegisters inFamily(): family id out of range");
        inr_assert(registerID < registers.size(),
                   "TargetRegisters inFamily(): register id out of range");
        return families[familyID].registers.find_bool(registerID);
    }

    bool inFamily(const RegisterFamily& family, uint32_t registerID) const {
        return inFamily(family.id, registerID);
    }

    bool inFamily(const RegisterFamily& family, const Register& reg) const {
        return inFamily(family, reg.id);
    }

    bool inFamily(uint32_t familyID, const Register& reg) const {
        return inFamily(familyID, reg.id);
    }

    /// @brief Returns whether or not the register is in that class.
    bool inClass(uint32_t classID, uint32_t registerID) const {
        inr_assert(classID < classes.size(),
                   "TargetRegisters inClass(): class id out of range");
        inr_assert(registerID < registers.size(),
                   "TargetRegisters inClass(): register id out of range");
        return std::binary_search(classes[classID].registers.begin(),
                                  classes[classID].registers.end(), registerID);
    }

    bool inClass(const RegisterClass& rclass, uint32_t registerID) const {
        return inClass(rclass.id, registerID);
    }

    bool inClass(const RegisterClass& rclass, const Register& reg) const {
        return inClass(rclass, reg.id);
    }

    bool inClass(uint32_t classID, const Register& reg) const {
        return inClass(classID, reg.id);
    }

    const RegisterClass* getClassOfType(inr::TIRT type) const {
        for(const auto& rc : classes) {
            if(rc.type == type) return &rc;
        }
        return nullptr;
    }

    const Register* getRegisterWithTypeFromFamily(inr::TIRT type,
                                                  uint32_t familyID) const {
        inr_assert(familyID < families.size(),
                   "TargetRegisters getRegisterWithTypeFromFamily(): family id "
                   "out of range");
        auto rcp = getClassOfType(type);
        if(!rcp) return nullptr;

        const RegisterFamily& rf = families[familyID];
        const RegisterClass& rc = *rcp;

        for(auto reg : rc.registers) {
            auto it = rf.registers.find(reg);
            if(it != rf.registers.end()) return &registers[*it];
        }
        return nullptr;
    }
};

} // namespace inr

#endif // INERTIA_TARGET_REGISTER
