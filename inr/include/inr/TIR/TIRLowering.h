// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TIRLOWERING_H
#define INERTIA_TIR_TIRLOWERING_H

/// @file TIR/TIRLowering.h
/// @brief Provides a class to lower SSA to TIR.

#include <inr/ADT/ArrView.h>
#include <inr/IR/Module.h>
#include <inr/Support/Stream.h>
#include <inr/TIR/TIRFunction.h>
#include <inr/TIR/TIRModule.h>
#include <inr/Target/CallingConv.h>
#include <inr/Target/Triple.h>

#include <forward_list>
#include <unordered_map>

namespace inr {

class TIRTargetDesc {
    /// @brief Native supported integer.
    ///
    /// For example on x86-64 it would be 64, 32, 16, 8.
    arrview<unsigned> intWords_;
    arrview<TIRInstInfo> instInfo_;

public:
    constexpr TIRTargetDesc(arrview<unsigned> intWords,
                            arrview<TIRInstInfo> table) noexcept :
        intWords_(intWords), instInfo_(table) {}

    constexpr arrview<unsigned> getIntWords() const noexcept {
        return intWords_;
    }

    constexpr bool isNativeInt(unsigned width) const noexcept {
        return std::find(intWords_.begin(), intWords_.end(), width);
    }

    bool isNativeInt(const Type* type) const noexcept {
        return type->isInteger() &&
               isNativeInt(((const IntegerType*)type)->getWidth());
    }

    constexpr arrview<TIRInstInfo> getInfoTable() const noexcept {
        return instInfo_;
    }

    constexpr bool enabledInst(TIRInstID id) const noexcept {
        return getInfoTable()[(unsigned)id].enabled;
    }
};

class TIRLowering {
    Triple triple_;
    const TIRTargetDesc* targetDesc_;
    std::forward_list<TIROperand> operandList_;
    std::unordered_map<const Value*, TIROperand*> operandMap_;

    void lowerSSAFunction(const Function& func, TIRModule* mod);
    void lowerSSABlock(const Block& block, TIRBlock* tblock,
                       CCStateGeneric& state, CallingConv cc);
    void lowerSSAValue(const Value* val, TIRBlock* block, CCStateGeneric& state,
                       CallingConv cc);
    void lowerSSAInstruction(const Instruction* inst, TIRBlock* block,
                             CCStateGeneric& state, CallingConv cc);
    void lowerSSABinaryInst(const BinaryInst* inst, TIRBlock* block);
    void lowerSSAAdd(const BinaryInst* inst, TIRBlock* block);
    void lowerSSARet(const Instruction* inst, TIRBlock* block, CallingConv cc);

    TIROperand* newOperand(TIROperand op) {
        return &operandList_.emplace_front(op);
    }

    TIROperand* mapOperand(const Value* key, TIROperand* op) {
        return operandMap_[key] = op;
    }

public:
    TIRLowering(Triple triple) noexcept :
        triple_(triple), targetDesc_(triple.getTIRTargetDesc()) {}

    std::unique_ptr<TIRModule> lowerSSA(const Module* mod);

    void print(const TIRModule* mod, raw_stream& os) const;

    Triple getTriple() const noexcept {
        return triple_;
    }

    const TIRTargetDesc* getDesc() const noexcept {
        return targetDesc_;
    }
};

} // namespace inr

#endif // INERTIA_TIR_TIRLOWERING_H
