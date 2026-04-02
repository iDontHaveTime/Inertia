// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/ADT/StrView.h>
#include <inr/IR/Context.h>
#include <inr/IR/Type.h>
#include <inr/Support/Stream.h>

#include <charconv>

namespace inr {

const Type* strToType(const InrContext& ctx, sview str) {
    if(str.empty()) return nullptr;
    if(str == "void") return ctx.getVoid();
    if(str == "ptr") return ctx.getPointer();
    if(str == "block") return ctx.getBlock();

    if(str[0] == 'i' && str.size() > 1) {
        sview iN = str.slice(1, str.size());
        unsigned N;
        auto res = std::from_chars(iN.begin(), iN.end(), N);
        if(res.ec != std::errc()) return nullptr;
        return ctx.getInt(N);
    }

    return nullptr;
}

std::string typeToStr(const Type* t) {
    string_stream ss;
    ss << *t;
    return ss.str();
}

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
        case Type::TypeID::Block:
            return os << "block";
    }
}

} // namespace inr