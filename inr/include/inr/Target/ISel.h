// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TARGET_ISEL_H
#define INERTIA_TARGET_ISEL_H

/// @file Target/ISel.h
/// @brief Contains the ISel base class.

#include <inr/DAG/DAGNode.h>
#include <inr/DAG/SelectionDAG.h>
#include <inr/IR/Block.h>
#include <inr/IR/Function.h>
#include <inr/IR/Module.h>
#include <inr/IR/Value.h>
#include <inr/MIR/MachineBlock.h>
#include <inr/MIR/MachineFunction.h>
#include <inr/MIR/MachineModule.h>
#include <inr/MIR/Register.h>
#include <inr/Target/CallingConv.h>
#include <inr/Target/Triple.h>

#include <unordered_map>

namespace inr {

/// @brief Base ISel class.
class ISel {
public:
    using ValueMap = std::unordered_map<const Value*, DAGNode*>;

protected:
    Triple triple_;
    const RegisterInfo* regInfo_;

    ISel(Triple triple) noexcept :
        triple_(triple), regInfo_(triple.getRegisterInfo()) {}

    virtual void lowerCall(const Instruction& inst, SelectionDAG& dag,
                           MachineBlock* mb) = 0;

    virtual void lowerReturn(const Instruction& inst, SelectionDAG& dag,
                             CallingConv cc, ValueMap& map,
                             MachineBlock* mb) = 0;

    virtual void matchEmit(DAGNode* dag, MachineBlock* block) = 0;

public:
    MachineModule* lower(const Module* module);
    MachineFunction* lowerFunction(const Function& function, MachineModule* mm);
    MachineBlock* lowerBlock(const Block& block, MachineFunction* mf,
                             SelectionDAG& selDag, CCState& state,
                             CallingConv cc);
    void lowerValue(const Value* val, SelectionDAG& selDag, CCState& state,
                    ValueMap& map);

    virtual ~ISel() noexcept = default;

    const Triple getTriple() const noexcept {
        return triple_;
    }

    const RegisterInfo* getRegisterInfo() const noexcept {
        return regInfo_;
    }
};

} // namespace inr

#endif // INERTIA_TARGET_ISEL_H
