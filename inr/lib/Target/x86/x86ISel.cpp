// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/DAG/DAGNode.h>
#include <inr/MIR/MachineInst.h>
#include <inr/MIR/Register.h>
#include <inr/Target/CallingConv.h>
#include <inr/Target/x86/x86ISel.h>
#define x86_NEED_OPCODES
#define x86_NEED_ISEL
#include <inr/Target/x86/x86Instructions.inc>

namespace inr::x86 {

void x86ISel::lowerCall(const Instruction&, SelectionDAG&, MachineBlock*) {}

void x86ISel::lowerReturn(const Instruction& inst, SelectionDAG& dag,
                          CallingConv cc, ValueMap& map, MachineFunction* mf) {
    DAGValue retinst = dag.createTarget((uint32_t)Opcodes::RET, inst.getType());

    if(!inst.getType()->isVoid()) {
        DAGValue retVal = map[inst.getOperand(0)];

        CCState state(mf, false, getRegisterInfo(),
                      getRegisterInfo()->getRegs());
        state.analyzeReturn(inst.getType(), getTriple().getCCRet(cc));

        const CCAssign& assign = state.getAssigns()[0];
        if(assign.isReg()) {
            DAGValue cpy = dag.createCopyToReg(retVal, assign.getDestReg(),
                                               assign.getDestType());
            retinst.getNode()->addOperand(cpy);
        }
    }

    dag.setRoot(retinst);
}

void x86ISel::matchEmit(DAGNode* dag, MachineBlock* block) {
    x86matchEmit(dag, block, getRegisterInfo());
}

} // namespace inr::x86