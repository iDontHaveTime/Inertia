// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/MIR/Register.h>
#include <inr/TIR/InstIt.h>
#include <inr/TIR/TIRLowering.h>

#include <utility>

namespace inr {

class LinearRegalloc {
    std::vector<std::pair<Register, bool>> allocated_;
    const RegisterInfo* reginfo_;

private:
    Register getSizedReg(unsigned size, Register reg) noexcept {
        auto getSizeBits = [&](Register r) {
            return reginfo_->getRegClass(r).getSize();
        };

        if(getSizeBits(reg) == size) return reg;

        // In cases like AH AL choose AL
        const Register* candidate = nullptr;
        if(reginfo_->hasSubRegs(reg)) {
            arrview<Register> subregs = reginfo_->getSubRegs(reg);
            for(const Register& subreg : subregs) {
                if(getSizeBits(subreg) == size) candidate = &subreg;
            }
        }

        return candidate ? *candidate : Register::createNone();
    }

public:
    LinearRegalloc(const RegisterInfo* reginfo,
                   arrview<Register> caller) noexcept :
        reginfo_(reginfo) {
        allocated_.reserve(caller.size());
        for(Register reg : caller) {
            allocated_.emplace_back(reg, false);
        }
    }

    std::pair<Register, unsigned> getRegisterWithSize(unsigned size) noexcept {
        unsigned idx = 0;
        for(auto& p : allocated_) {
            if(p.second) {
                idx++;
                continue;
            }
            Register reg = getSizedReg(size, p.first);
            if(!reg.isNone()) p.second = true;
            return {reg, idx};
        }
        return {};
    }

    bool isAllocated(Register reg) noexcept {
        for(const auto& p : allocated_) {
            if(p.first == reg) return p.second;
        }
        inr_notpossible(
            "Linear regalloc requested a register that doesn't exist.");
    }

    void markFree(Register reg) noexcept {
        for(auto& p : allocated_) {
            if(p.first == reg) {
                p.second = false;
                return;
            }
        }
    }

    void markAllocated(Register reg) noexcept {
        for(auto& p : allocated_) {
            if(p.first == reg) {
                p.second = true;
                return;
            }
        }
    }

    Register getOriginalReg(Register reg) const noexcept {
        for(const auto& p : allocated_) {
            if(p.first == reg) return reg;
            if(reginfo_->hasSuperRegs(reg)) {
                arrview<Register> sregs = reginfo_->getSuperRegs(reg);
                for(const Register& sreg : sregs) {
                    if(sreg == p.first) return sreg;
                }
            }
            else if(reginfo_->hasSubRegs(reg)) {
                arrview<Register> sregs = reginfo_->getSubRegs(reg);
                for(const Register& sreg : sregs) {
                    if(sreg == p.first) return sreg;
                }
            }
        }
        return {};
    }

    Register getReg(unsigned n) const noexcept {
        return allocated_[n].first;
    }
};

bool TIRLowering::linearRegallocFunction(TIRFunction* func) {
    // Track at what instructions operands start and when they end.
    std::unordered_map<TIROperand*, unsigned> liveMap;

    size_t instructionIndex = 0;
    InstructionIterator iit(func->getBlocks().front());

    for(TIRInstruction& inst : iit) {
        for(TIROperand* op : inst.getOperands()) {
            liveMap[op] = instructionIndex;
        }

        instructionIndex++;
    }

    CallingConv cc = func->getFunction()->getCC();
    if(cc == CallingConv::C) {
        cc = triple_.getDefaultCC();
    }

    arrview<Register> caller = triple_.getCallerSaved(cc);
    LinearRegalloc regalloc(triple_.getRegisterInfo(), caller);
    std::vector<std::pair<unsigned, Register>> whenToRemove;
    whenToRemove.reserve(liveMap.size());

    instructionIndex = 0;
    for(TIRInstruction& inst : iit) {
        for(auto it = whenToRemove.begin(); it != whenToRemove.end(); ++it) {
            const auto& p = *it;
            if(p.first == instructionIndex) {
                regalloc.markFree(p.second);
                whenToRemove.erase(it);
                break;
            }
        }

        const Type* instType = inst.getType();

        if(instType->isInteger()) {
            const IntegerType* iType = (const IntegerType*)instType;
            for(TIROperand*& op : inst.getOperands()) {
                unsigned life = liveMap[op];
                if(op->isReg()) {
                    Register opReg = op->getReg();
                    if(opReg.isVirtual()) {
                        std::pair<Register, unsigned> reg =
                            regalloc.getRegisterWithSize(iType->getWidth());
                        if(!reg.first.isNone()) {
                            whenToRemove.emplace_back(
                                life, regalloc.getReg(reg.second));
                            *op = TIROperand(reg.first);
                        }
                        else {
                            // Spill
                        }
                    }
                    else if(opReg.isPhysical()) {
                        Register reg = regalloc.getOriginalReg(opReg);
                        if(!reg.isNone()) {
                            regalloc.markAllocated(reg);
                            whenToRemove.emplace_back(life, reg);
                        }
                    }
                }
            }
        }

        instructionIndex++;
    }

    return false;
}

void TIRLowering::linearRegallocPass(TIRModule* mod) {
    if(!mod) return;

    for(TIRFunction& func : mod->getFunctions()) {
        while(linearRegallocFunction(&func));
    }
}

} // namespace inr