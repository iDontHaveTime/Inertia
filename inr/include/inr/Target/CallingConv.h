// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TARGET_CALLINGCONV_H
#define INERTIA_TARGET_CALLINGCONV_H

/// @file Target/CallingConv.h
/// @brief Calling convention related classes live here.

#include <inr/ADT/ArrView.h>
#include <inr/ADT/IVector.h>
#include <inr/IR/Type.h>
#include <inr/MIR/MachineFunction.h>
#include <inr/MIR/Register.h>
#define INR_CCFUNC
#include <inr/Target/CCFunc.h>

#include <cstdint>
#include <initializer_list>
#include <variant>
#include <vector>

namespace inr {

class CCAssign {
public:
    enum class Info {
        Full, ///< Value fits in the location.

        SignExt, ///< Value sign extended in the location.
        ZeroExt, ///< Value zero extended in the location.
        UdefExt, ///< Value extended but with undefined bits.

        Trunc ///< Value is truncated.

    };

private:
    /// @brief What data is the dest assigned to.
    ///
    /// Can be a register (e.g. RDI arg 1).
    /// Or can also be a frame index (e.g. 8(%rsp)).
    std::variant<Register, int64_t> data_;
    /// @brief Info about the dest.
    Info info_;
    /// @brief Used for extra info (like arg number).
    unsigned valN_;
    /// @brief Type of the source.
    const Type* src_;
    /// @brief Type of the dest.
    const Type* dest_;

    CCAssign(Info info, unsigned valN, const Type* src,
             const Type* dest) noexcept :
        data_(0), info_(info), valN_(valN), src_(src), dest_(dest) {}

public:
    static CCAssign getReg(unsigned valN, const Type* src, Register reg,
                           const Type* dest, Info info) {
        CCAssign as(info, valN, src, dest);
        as.data_ = reg;
        return as;
    }

    static CCAssign getMem(unsigned valN, const Type* src, int64_t off,
                           const Type* dest, Info info) {
        CCAssign as(info, valN, src, dest);
        as.data_ = off;
        return as;
    }

    bool isReg() const noexcept {
        return std::holds_alternative<Register>(data_);
    }

    bool isMem() const noexcept {
        return std::holds_alternative<int64_t>(data_);
    }

    unsigned getValN() const noexcept {
        return valN_;
    }

    Register getDestReg() const {
        return std::get<Register>(data_);
    }

    int64_t getDestMem() const {
        return std::get<int64_t>(data_);
    }

    const Type* getSrcType() const noexcept {
        return src_;
    }

    const Type* getDestType() const noexcept {
        return dest_;
    }

    Info getInfo() const noexcept {
        return info_;
    }
};

class CCState {
    MachineFunction* mfunc_;
    bool vararg_;
    const RegisterInfo* regInfo_;
    std::vector<CCAssign> assigns_;

    std::vector<Register> availableRegs_;
    int64_t stackOffset_ = 0;

public:
    CCState(MachineFunction* mfunc, bool vararg, const RegisterInfo* regInfo,
            arrview<Register> allocateable) :
        mfunc_(mfunc),
        vararg_(vararg),
        regInfo_(regInfo),
        availableRegs_(allocateable.begin(), allocateable.end()) {}

    arrview<CCAssign> getAssigns() const noexcept {
        return assigns_;
    }

    bool isVararg() const noexcept {
        return vararg_;
    }

    const MachineFunction* getMFunc() const noexcept {
        return mfunc_;
    }

    arrview<Register> getAvailableRegs() const noexcept {
        return availableRegs_;
    }

    int64_t getStackOffset() const noexcept {
        return stackOffset_;
    }

    Register allocateReg(std::initializer_list<Register> regs) {
        return allocateReg(arrview<Register>(regs.begin(), regs.end()));
    }

    Register allocateReg(arrview<Register> regs) {
        for(Register reg : regs) {
            auto it =
                std::find(availableRegs_.begin(), availableRegs_.end(), reg);
            if(it != availableRegs_.end()) {
                availableRegs_.erase(it);

                for(Register sub : regInfo_->getSubRegs(reg))
                    availableRegs_.erase(std::remove(availableRegs_.begin(),
                                                     availableRegs_.end(), sub),
                                         availableRegs_.end());
                for(Register super : regInfo_->getSuperRegs(reg))
                    availableRegs_.erase(
                        std::remove(availableRegs_.begin(),
                                    availableRegs_.end(), super),
                        availableRegs_.end());
                return reg;
            }
        }
        return Register::createNone();
    }

    int64_t allocateStack(unsigned size, unsigned align) {
        stackOffset_ = (stackOffset_ + align - 1) & ~(align - 1);
        int64_t offset = stackOffset_;
        stackOffset_ += size;
        return offset;
    }

    void analyzeArgs(arrview<const Type*> args, CCFunc ccFunc) {
        for(unsigned i = 0; i < args.size(); i++) {
            ccFunc(i, args[i], *this);
        }
    }

    void analyzeReturn(const Type* ret, CCFunc ccFunc) {
        ccFunc(0, ret, *this);
    }

    void addAssign(CCAssign assign) {
        assigns_.emplace_back(assign);
    }
};

} // namespace inr

#endif // INERTIA_TARGET_CALLINGCONV_H
