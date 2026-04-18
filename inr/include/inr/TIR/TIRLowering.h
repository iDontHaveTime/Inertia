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
#include <inr/Target/Flags.h>
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
    Register stackRegister_; ///< Stack register, assume callee saved.
    Register frameRegister_; ///< Frame register, assume callee saved.

public:
    constexpr TIRTargetDesc(arrview<unsigned> intWords,
                            arrview<TIRInstInfo> table, Register stackRegister,
                            Register frameRegister = {}) noexcept :
        intWords_(intWords),
        instInfo_(table),
        stackRegister_(stackRegister),
        frameRegister_(frameRegister) {}

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

    constexpr TIRInstInfo getInfo(TIRInstID id) const noexcept {
        return getInfoTable()[(unsigned)id];
    }

    constexpr bool enabledInst(TIRInstID id) const noexcept {
        return getInfo(id).getEnabled();
    }

    constexpr Register getFrameRegister() const noexcept {
        return frameRegister_;
    }

    constexpr Register getStackRegister() const noexcept {
        return stackRegister_;
    }
};

class TIRLowering {
    InrContext& ctx_;
    Triple triple_;
    Flags flags_;
    bool stackKnownAtCT_ =
        true; ///< Whether or not the stack size is known at compile time.
    TIROperand* stackRegister_;
    TIROperand*
        pointerConstant_; ///< Integer with the same byte size as the pointer.
    const IntegerType* ptrAsInteger_;
    const TIRTargetDesc* targetDesc_;
    std::forward_list<TIROperand> operandList_;
    std::unordered_map<const Value*, TIROperand*> operandMap_;

    void lowerSSAFunction(const Function& func, TIRModule* mod);
    void lowerSSABlock(const Block& block, TIRBlock* tblock,
                       CCStateGeneric& state);
    void lowerSSAValue(const Value* val, TIRBlock* block,
                       CCStateGeneric& state);
    void lowerSSAInstruction(const Instruction* inst, TIRBlock* block,
                             CCStateGeneric& state);
    void lowerSSABinaryInst(const BinaryInst* inst, TIRBlock* block);
    void lowerSSAAdd(const BinaryInst* inst, TIRBlock* block);
    void lowerSSARet(const Instruction* inst, TIRBlock* block);
    void emitStore(const Type* type, TIROperand* dest, TIROperand* src,
                   TIRBlock* block);
    void emitAdd(const Type* type, TIROperand* dest, TIROperand* lhs,
                 TIROperand* rhs, TIRBlock* block);
    void emitSub(const Type* type, TIROperand* dest, TIROperand* lhs,
                 TIROperand* rhs, TIRBlock* block);
    void emitPrologue(TIRBlock* block);
    void emitEpilogue(TIRBlock* block);
    void scanFunction(TIRFunction* func);

    /// @brief Any variant of the same operation will work.
    ///
    /// For example it would accept both ADD_DEST_SRC and ADD_DEST_SRC_SRC as
    /// ADD.
    void emitArithmetic(TIRInstID op, const Type* type, TIROperand* dest,
                        TIROperand* lhs, TIROperand* rhs, TIRBlock* block);

    void lowerSSAAlloca(const AllocaInst* inst, TIRBlock* block);

    void solveOperands(const Type* type, TIROperand*& dest,
                       ncarrview<TIROperand*> srcs, TIRInstID tirInst,
                       TIRBlock* block);
    TIROperand* solveOperand(const Type* type, TIROperand* operand,
                             TIRInstInfo::OperandAllowed allowed,
                             TIRBlock* block);

    static bool isAllowed(TIROperand* op,
                          TIRInstInfo::OperandAllowed allowed) noexcept;

    TIROperand* newOperand(TIROperand op) {
        return &operandList_.emplace_front(op);
    }

    TIROperand* newRegOp(Register reg) {
        return newOperand(reg);
    }

    TIROperand* newMemReg(Register reg, int64_t off = 0) {
        return newOperand(MemOperand(reg, off));
    }

    TIROperand* mapOperand(const Value* key, TIROperand* op) {
        return operandMap_[key] = op;
    }

    TIROperand* newVreg(TIRFunction* func) {
        return newOperand(Register::createVirtual(func->allocateVreg()));
    }

    TIROperand* newVreg(TIRBlock* block) {
        return newVreg(block->getParent());
    }

    bool linearRegallocFunction(TIRFunction* func);

public:
    TIRLowering(InrContext& ctx, Triple triple, Flags flags) noexcept :
        ctx_(ctx),
        triple_(triple),
        flags_(flags),
        targetDesc_(triple.getTIRTargetDesc()) {}

    std::unique_ptr<TIRModule> lowerSSA(const Module* mod);
    void linearRegallocPass(TIRModule* mod);

    void linearRegallocPass(std::unique_ptr<TIRModule>& mod) {
        linearRegallocPass(mod.get());
    }

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
