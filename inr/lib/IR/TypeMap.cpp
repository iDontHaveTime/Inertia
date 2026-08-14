// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/ADT/ArrView.h>
#include <inr/ADT/HMap.h>
#include <inr/ADT/HSet.h>
#include <inr/IR/Type.h>
#include <inr/IR/TypeMap.h>
#include <inr/Math/FPFormat.h>
#include <inr/Support/Assert.h>

#include <memory>
#include <new>

namespace inr {

struct FuncLookup {
    const Type* ret;
    arrview<const Type*> args;
    bool vararg;
};

template<>
struct HMapInfo<std::unique_ptr<FuncType>> {
    static std::size_t hash(const std::unique_ptr<FuncType>& ft) {
        std::size_t seed = HMapInfo<const Type*>::hash(ft->getReturn());
        seed ^= std::size_t(ft->isVararg()) + 0x9e3779b9 + (seed << 6) +
                (seed >> 2);
        for(unsigned i = 0; i < ft->getNumArgs(); i++) {
            seed ^= HMapInfo<const Type*>::hash(ft->getArg(i)) + 0x9e3779b9 +
                    (seed << 6) + (seed >> 2);
        }
        return seed;
    }

    static std::size_t hash(const FuncLookup& ft) {
        std::size_t seed = HMapInfo<const Type*>::hash(ft.ret);
        seed ^= std::size_t(ft.vararg) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        for(unsigned i = 0; i < ft.args.size(); i++) {
            seed ^= HMapInfo<const Type*>::hash(ft.args[i]) + 0x9e3779b9 +
                    (seed << 6) + (seed >> 2);
        }
        return seed;
    }

    static bool equal(const FuncLookup& lhs,
                      const std::unique_ptr<FuncType>& rhs) {
        if(lhs.args.size() != rhs->getNumArgs()) return false;
        if(lhs.vararg != rhs->isVararg()) return false;

        for(unsigned i = 0; i < lhs.args.size(); i++) {
            if(lhs.args[i] != rhs->getArg(i)) return false;
        }

        return true;
    }

    static bool equal(const std::unique_ptr<FuncType>& lhs,
                      const std::unique_ptr<FuncType>& rhs) {
        if(lhs->getNumArgs() != rhs->getNumArgs()) return false;
        if(lhs->isVararg() != rhs->isVararg()) return false;

        for(unsigned i = 0; i < lhs->getNumArgs(); i++) {
            if(lhs->getArg(i) != rhs->getArg(i)) return false;
        }

        return true;
    }
};

class TypeMapInternal {
    HMap<unsigned, std::unique_ptr<IntType>> integerMap_;
    HSet<std::unique_ptr<FuncType>> funcsSet_;

public:
    const VoidType* getVoidType() const {
        constexpr static VoidType vt_;
        return &vt_;
    }

    const PtrType* getPtrType() const {
        constexpr static PtrType ptr_;
        return &ptr_;
    }

    const BlockType* getBlockType() const {
        constexpr static BlockType bt_;
        return &bt_;
    }

    const FPType* getFPType(FPFormat fmt) const {
        constexpr static FPType b16_{FPFormat::Binary16},
            b32_{FPFormat::Binary32}, b64_{FPFormat::Binary64},
            b80_{FPFormat::x87_80};
        switch(fmt) {
            case FPFormat::Binary16:
                return &b16_;
            case FPFormat::Binary32:
                return &b32_;
            case FPFormat::Binary64:
                return &b64_;
            case FPFormat::x87_80:
                return &b80_;
        }
    }

    const IntType* getIntType(unsigned width) {
        constexpr static IntType i1_{1}, i8_(8), i16_(16), i32_(32), i64_(64);
        switch(width) {
            case 1:
                return &i1_;
            case 8:
                return &i8_;
            case 16:
                return &i16_;
            case 32:
                return &i32_;
            case 64:
                return &i64_;
            default:
                break;
        }

        auto [val, emp] = integerMap_.try_emplace(width, nullptr);
        if(emp) {
            val->reset(new IntType(width));
        }
        return val->get();
    }

    const FuncType* getFuncType(const Type* ret, arrview<const Type*> args,
                                bool vararg) {
        FuncLookup lookup(ret, args, vararg);

        if(auto v = funcsSet_.find(lookup)) {
            return v->get();
        }

        auto [v, e] = funcsSet_.try_emplace(
            std::unique_ptr<FuncType>(new FuncType(ret, args, vararg)));

        inr_assert(e,
                   "TypeMapInternal getFuncType(): the type must be emplaced");
        return v->get();
    }
};

TypeMap::TypeMap() : internal_(new TypeMapInternal()) {}

TypeMap::~TypeMap() {
    delete internal_;
}

TypeMap::TypeMap(TypeMap&& other) noexcept : internal_(other.internal_) {
    other.internal_ = nullptr;
}

TypeMap& TypeMap::operator=(TypeMap&& other) noexcept {
    if(this != &other) {
        delete internal_;
        internal_ = other.internal_;
        other.internal_ = nullptr;
    }
    return *this;
}

const IntType* TypeMap::getI1() {
    return getInt(1);
}

const IntType* TypeMap::getI8() {
    return getInt(8);
}

const IntType* TypeMap::getI16() {
    return getInt(16);
}

const IntType* TypeMap::getI32() {
    return getInt(32);
}

const IntType* TypeMap::getI64() {
    return getInt(64);
}

const IntType* TypeMap::getInt(unsigned width) {
    return internal_->getIntType(width);
}

const VoidType* TypeMap::getVoid() {
    return internal_->getVoidType();
}

const PtrType* TypeMap::getPtr() {
    return internal_->getPtrType();
}

const BlockType* TypeMap::getBlock() {
    return internal_->getBlockType();
}

const FuncType* TypeMap::getFunc(const Type* ret, arrview<const Type*> args,
                                 bool vararg) {
    inr_assert(ret != nullptr, "TypeMap getFunc(): passed in a nullptr ret");
    return internal_->getFuncType(ret, args, vararg);
}

const FPType* TypeMap::getFloat(FPFormat fmt) {
    return internal_->getFPType(fmt);
}

} // namespace inr
