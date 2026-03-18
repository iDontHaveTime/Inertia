// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/IR/Constant.h>
#include <inr/IR/Instruction.h>
#include <inr/IR/Value.h>

namespace inr {

raw_stream& operator<<(raw_stream& os, const Value& value) {
    switch(value.getValueID()) {
        case Value::ValueID::Argument:
            return os << '%' << value.getName();
        case Value::ValueID::Function:
            return os << '@' << value.getName();
        case Value::ValueID::ConstantInt:
            return os << ((const ConstantInt&)value).getValue();
        case Value::ValueID::Instruction:
            return os << '%' << ((const Instruction&)value).getName();
        default:
            return os;
    }
}

} // namespace inr