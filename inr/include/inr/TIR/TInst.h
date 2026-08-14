// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TINST_H
#define INERTIA_TIR_TINST_H

/// @file TIR/TInst.h
/// @brief Provides a TIR instruction class.

#include <inr/ADT/ArrView.h>
#include <inr/ADT/IList.h>
#include <inr/ADT/IVector.h>
#include <inr/TIR/TIRT.h>
#include <inr/TIR/TOperand.h>

namespace inr {

class TInst : public ilist_node<TInst> {
public:
    /// @brief Generic instruction types.
    enum InstType : unsigned {
        gRet,
        gJmp,
        gCmp,
        gPhi,
        gAdd,
        gSub,
        gLoad,
        gStore,
        gAlloca,
        gCopy,
        gMul,
        gUDiv,
        gSDiv,
        gURem,
        gSRem,
        gShl,
        gLShr,
        gAShr,
        gAnd,
        gOr,
        gXor,
        gInteger,
        gUndef,
        GENERIC_LAST,
    };

private:
    InstType instType_;
    TIRT type_;
    ivec<TOperand, 3> operands_;

public:
    TInst(InstType instType, TIRT type) : instType_(instType), type_(type) {}

    void addOperand(TOperand op) {
        operands_.emplace_back(op);
    }

    arrview<TOperand> getOperands() const {
        return operands_;
    }

    InstType getInstType() const {
        return instType_;
    }

    TIRT getType() const {
        return type_;
    }
};

} // namespace inr

#endif // INERTIA_TIR_TINST_H
