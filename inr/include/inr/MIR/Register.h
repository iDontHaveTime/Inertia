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

    constexpr static uint32_t VIRT_START = 0x0000FFFF;
    constexpr static uint32_t NONE = ~0;

private:
    /// @brief Used for virtual and physical registers.
    ///
    /// Physical is less than 0x0000FFFF.
    /// Virtual is more equals 0x0000FFFF.
    /// None is 0xFFFFFFFF.
    uint32_t index_;

    constexpr Register(uint32_t index) noexcept : index_(index) {}

public:
    constexpr Kind getKind() const noexcept {
        if(isPhysical()) return Kind::Physical;
        if(isVirtual()) return Kind::Virtual;
        return Kind::None;
    }

    constexpr bool isVirtual() const noexcept {
        return index_ >= VIRT_START;
    }

    constexpr bool isPhysical() const noexcept {
        return index_ < VIRT_START;
    }

    constexpr bool isNone() const noexcept {
        return index_ == NONE;
    }

    constexpr uint32_t getIndex() const noexcept {
        if(isVirtual()) return index_ - VIRT_START;
        return index_;
    }

    constexpr uint32_t getRawIndex() const noexcept {
        return index_;
    }

    constexpr Register() noexcept : index_(NONE) {}

    constexpr Register(uint32_t index, Kind kind) noexcept :
        index_(kind == Kind::Virtual ? index + VIRT_START : index) {}

    constexpr static Register createPhysical(uint32_t index) noexcept {
        return Register(index);
    }

    constexpr static Register createVirtual(uint32_t index) noexcept {
        return Register(index + VIRT_START);
    }

    constexpr static Register createNone() noexcept {
        return Register(NONE);
    }

    constexpr bool operator==(Register other) const noexcept {
        return other.index_ == index_;
    }

    constexpr Register(const Register&) noexcept = default;
    constexpr Register& operator=(const Register&) noexcept = default;

    constexpr Register(Register&&) noexcept = default;
    constexpr Register& operator=(Register&&) noexcept = default;
};

class MemOperand {
    Register reg_;   ///< Underlying register.
    int64_t offset_; ///< Offset of the memory.
public:
    MemOperand(Register reg, int64_t offset) noexcept :
        reg_(reg), offset_(offset) {}

    int64_t getOffset() const noexcept {
        return offset_;
    }

    Register getRegister() const noexcept {
        return reg_;
    }

    bool operator==(const MemOperand&) const noexcept = default;
};

class RegisterClass {
    uint32_t regStart_;
    uint32_t regEnd_;
    unsigned size_;
    uint32_t name_;

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

    constexpr uint32_t getName() const noexcept {
        return name_;
    }

public:
    constexpr RegisterClass(uint32_t start, uint32_t end, unsigned size,
                            uint32_t name) noexcept :
        regStart_(start), regEnd_(end), size_(size), name_(name) {}

    constexpr unsigned getSize() const noexcept {
        return size_;
    }

    friend class RegisterInfo;
};

class RegisterDesc {
    uint32_t name_;
    uint32_t subregs_;
    uint32_t superregs_;
    uint16_t subregsC_;
    uint16_t superregsC_;

    constexpr uint32_t getName() const noexcept {
        return name_;
    }

    constexpr uint16_t getSubRegC() const noexcept {
        return subregsC_;
    }

    constexpr uint32_t getSubRegs() const noexcept {
        return subregs_;
    }

    constexpr uint16_t getSuperRegC() const noexcept {
        return superregsC_;
    }

    constexpr uint32_t getSuperRegs() const noexcept {
        return superregs_;
    }

    constexpr bool hasSubRegs() const noexcept {
        return subregsC_ != 0;
    }

    constexpr bool hasSuperRegs() const noexcept {
        return superregsC_ != 0;
    }

public:
    constexpr RegisterDesc(uint32_t name, uint32_t subregs, uint32_t superregs,
                           uint16_t subregsC, uint16_t superregsC) noexcept :
        name_(name),
        subregs_(subregs),
        superregs_(superregs),
        subregsC_(subregsC),
        superregsC_(superregsC) {}

    friend class RegisterInfo;
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

    constexpr void checkReg(Register reg) const noexcept {
        inr_assert(!reg.isVirtual(),
                   "Virtual register is not a physical register.");
        inr_assert(reg.getIndex() < regNum_, "Register is out of bounds.");
        (void)reg;
    }

    constexpr arrview<Register> getSubRegs(Register reg) const noexcept {
        checkReg(reg);

        RegisterDesc desc = descArray_[reg.getIndex()];
        return {regsArray_.data() + desc.getSubRegs(), desc.getSubRegC()};
    }

    constexpr arrview<Register> getSuperRegs(Register reg) const noexcept {
        checkReg(reg);

        RegisterDesc desc = descArray_[reg.getIndex()];
        return {regsArray_.data() + desc.getSuperRegs(), desc.getSuperRegC()};
    }

    constexpr bool hasSuperRegs(Register reg) const noexcept {
        checkReg(reg);
        return descArray_[reg.getIndex()].hasSuperRegs();
    }

    constexpr bool hasSubRegs(Register reg) const noexcept {
        checkReg(reg);
        return descArray_[reg.getIndex()].hasSubRegs();
    }

    constexpr arrview<Register> getRegClassRegs(
        RegisterClass regclass) const noexcept {
        return regclass.getRegs(regsArray_.data());
    }

    constexpr sview getName(Register reg) const noexcept {
        checkReg(reg);

        return {strArray_.data() + descArray_[reg.getIndex()].getName()};
    }

    constexpr sview getName(RegisterClass regclass) const noexcept {
        return {strArray_.data() + regclass.getName()};
    }

    constexpr RegisterClass getRegClass(unsigned idx) const noexcept {
        return classArray_[idx];
    }

    constexpr RegisterClass getRegClass(sview name) const noexcept {
        for(const RegisterClass& regclass : classArray_) {
            if((strArray_.data() + regclass.getName()) == name) return regclass;
        }
        return {0, 0, 0, 0};
    }

    constexpr RegisterClass getRegClass(Register reg) const noexcept {
        for(const RegisterClass& regclass : classArray_) {
            auto rarray = regclass.getRegs(regsArray_.data());
            if(std::find(rarray.begin(), rarray.end(), reg) != rarray.end())
                return regclass;
        }
        inr_notpossible("Register class not found.");
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

    constexpr bool inRegClass(RegisterClass regclass,
                              Register reg) const noexcept {
        if(regclass.regStart_ == 0 && regclass.regEnd_ == 0 &&
           regclass.size_ == 0 && regclass.name_ == 0)
            return false;
        if(reg.isVirtual()) return true;

        return std::find(regsArray_.data() + regclass.getStart(),
                         (regsArray_.data() + regclass.getStart()) +
                             regclass.getEnd(),
                         reg) !=
               ((regsArray_.data() + regclass.getStart()) + regclass.getEnd());
    }

    constexpr arrview<Register> getRegs() const noexcept {
        return {regsArray_.data(), regNum_};
    }
};

} // namespace inr

#endif // INERTIA_MIR_REGISTER_H
