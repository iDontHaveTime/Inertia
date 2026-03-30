// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/IR/Type.h>
#include <inr/Support/Stream.h>

namespace inr {

raw_stream& operator<<(raw_stream& os, const Type& type) {
    switch(type.getTypeID()) {
        case Type::TypeID::Void:
            return os << "void";
        case Type::TypeID::Integer:
            return os << 'i' << ((const IntegerType&)type).getWidth();
        case Type::TypeID::Pointer:
            return os << "ptr";
        case Type::TypeID::Function: {
            const auto& fn = (const FunctionType&)type;
            os << '(' << *fn.getReturn() << ")(";
            size_t argc = fn.getArgs().size();
            for(size_t i = 0; i < argc; i++) {
                if(i) os << ", ";
                os << *fn.getArgs()[i];
            }
            return os << ')';
        }
        default:
            return os << "unknown";
    }
}

} // namespace inr