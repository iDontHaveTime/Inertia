// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_MIR_REGISTER_H
#define INERTIA_MIR_REGISTER_H

/// @file MIR/Register.h
/// @brief Contains the register class.

#include <inr/ADT/ArrView.h>
#include <inr/ADT/StrView.h>

#include <cstdint>

namespace inr {

/// @brief Represents a physical or virtual register.
class Register {
public:
    enum class Kind : uint8_t { None, Physical, Virtual };

    constexpr static uint32_t NONE = ~0;

private:
    uint32_t index_;
    Kind kind_;

public:
    constexpr Kind getKind() const noexcept {
        return kind_;
    }

    constexpr bool isVirtual() const noexcept {
        return kind_ == Kind::Virtual;
    }

    constexpr bool isPhysical() const noexcept {
        return kind_ == Kind::Physical;
    }

    constexpr bool isNone() const noexcept {
        return kind_ == Kind::None || index_ == NONE;
    }

    constexpr uint32_t getIndex() const noexcept {
        return index_;
    }

    constexpr Register(uint32_t index, Kind kind) noexcept :
        index_(index), kind_(kind) {}

    constexpr static Register createPhysical(uint32_t index) noexcept {
        return Register(index, Kind::Physical);
    }

    constexpr static Register createVirtual(uint32_t index) noexcept {
        return Register(index, Kind::Virtual);
    }

    constexpr static Register createNone() noexcept {
        return Register(NONE, Kind::None);
    }

    constexpr bool operator==(Register other) const noexcept {
        return other.index_ = index_ && other.kind_ == kind_;
    }

    constexpr Register(const Register&) noexcept = default;
    constexpr Register& operator=(const Register&) noexcept = default;

    constexpr Register(Register&&) noexcept = default;
    constexpr Register& operator=(Register&&) noexcept = default;
};

class RegisterClass {
    uint32_t regStart_;
    uint32_t regEnd_;
    unsigned size_;
    uint32_t name_;

public:
    constexpr RegisterClass(uint32_t start, uint32_t end, unsigned size,
                            uint32_t name) noexcept :
        regStart_(start), regEnd_(end), size_(size), name_(name) {}

    constexpr uint32_t getStart() const noexcept {
        return regStart_;
    }

    constexpr uint32_t getEnd() const noexcept {
        return regEnd_;
    }

    constexpr arrview<Register> getRegs(
        const Register* regarray) const noexcept {
        return {regarray, getStart(), getEnd()};
    }

    constexpr Register getReg(const Register* regarray,
                              unsigned reg) const noexcept {
        return *(regarray + regStart_ + reg);
    }

    constexpr unsigned getSize() const noexcept {
        return size_;
    }

    constexpr uint32_t getName() const noexcept {
        return name_;
    }
};

class RegisterDesc {
    uint32_t name_;
    uint32_t subregs_;
    uint32_t subregsC_;

public:
    constexpr uint32_t getName() const noexcept {
        return name_;
    }

    constexpr uint32_t getSubRegC() const noexcept {
        return subregsC_;
    }

    constexpr uint32_t getSubRegs() const noexcept {
        return subregs_;
    }

    constexpr bool hasSubRegs() const noexcept {
        return subregs_ != 0;
    }

    constexpr RegisterDesc(uint32_t name, uint32_t subregs,
                           uint32_t subregsC) noexcept :
        name_(name), subregs_(subregs), subregsC_(subregsC) {}
};

class RegisterInfo {
    sview strArray_;
    arrview<Register> regsArray_;
    unsigned regNum_;
    arrview<RegisterDesc> descArray_;
    arrview<RegisterClass> classArray_;

public:
    constexpr RegisterInfo(sview strArray, arrview<Register> regsArray,
                           unsigned regNum, arrview<RegisterDesc> descArray,
                           arrview<RegisterClass> classArray) noexcept :
        strArray_(strArray),
        regsArray_(regsArray),
        regNum_(regNum),
        descArray_(descArray),
        classArray_(classArray) {}

    constexpr bool checkReg(Register reg) const noexcept {
        if(reg.isVirtual() || reg.getIndex() >= regNum_) return true;

        return false;
    }

    constexpr arrview<Register> getSubRegs(Register reg) const noexcept {
        if(checkReg(reg)) return {};

        RegisterDesc desc = descArray_[reg.getIndex()];
        return {regsArray_.data() + desc.getSubRegs(), desc.getSubRegC()};
    }

    constexpr arrview<Register> getRegClassRegs(
        RegisterClass regclass) const noexcept {
        return regclass.getRegs(regsArray_.data());
    }

    constexpr sview getName(Register reg) const noexcept {
        if(checkReg(reg)) return {};

        return {strArray_.data() + descArray_[reg.getIndex()].getName()};
    }

    constexpr sview getName(RegisterClass regclass) const noexcept {
        return {strArray_.data() + regclass.getName()};
    }

    constexpr RegisterClass getRegClass(unsigned idx) const noexcept {
        return classArray_[idx];
    }

    constexpr RegisterClass getRegClass(sview name) const {
        for(const RegisterClass& regclass : classArray_) {
            if((strArray_.data() + regclass.getName()) == name) return regclass;
        }
        return {0, 0, 0, 0};
    }

    constexpr Register getByName(sview name) const noexcept {
        if(name.empty()) return Register::createNone();
        unsigned idx = 0;
        for(RegisterDesc desc : descArray_) {
            char c = strArray_[desc.getName()];
            if(c == name[0]) {
                const char* candidate = strArray_.data() + desc.getName();
                if(name == candidate) {
                    return regsArray_[idx];
                }
            }
            idx++;
        }
        return Register::createNone();
    }
};

} // namespace inr

#endif // INERTIA_MIR_REGISTER_H
