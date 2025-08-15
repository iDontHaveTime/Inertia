#ifndef INERTIA_IRBUILDER_HPP
#define INERTIA_IRBUILDER_HPP

#include "Inertia/IR/Frame.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/IR/Type.hpp"
#include <cstddef>

#ifdef private
#undef private
#endif

namespace Inertia{
    /* The builder returns this so that you wont access raw references */
    class __InternalIRPack__{
        enum class IRPackType{
            FUNCTION,
        } type;
        size_t index;
        
        __InternalIRPack__() noexcept = default;
        __InternalIRPack__(IRPackType t, size_t i) noexcept : type(t), index(i){};

        size_t getIndex() const noexcept{
            return index;
        }
        IRPackType getType() const noexcept{
            return type;
        }

        friend class IRBuilder;
        friend class IRPack;
    };
    class IRPack{
        size_t i;

        size_t getIndex() const noexcept{
            return i;
        }

        IRPack() noexcept = default;
        IRPack(size_t index) noexcept : i(index){};
        friend class IRBuilder;
    };
    class IRBuilder{
        std::vector<__InternalIRPack__> packs;
        TypeAllocator* talloc;
        Frame* frame;
    public:

        IRBuilder() noexcept : talloc(nullptr), frame(nullptr){};

        IRBuilder(TypeAllocator* tpalloc, Frame* frm) noexcept : talloc(tpalloc), frame(frm){};

        const Frame* getFrame() const noexcept{
            return frame;
        }
        const TypeAllocator* getAllocator() const noexcept{
            return talloc;
        }

        IRPack buildFunction(){
            if(!frame) return IRPack();
            size_t _func_index_ = frame->funcs.size();

            size_t _pack_index_ = packs.size();
            packs.push_back({__InternalIRPack__::IRPackType::FUNCTION, _func_index_});
            return IRPack(_pack_index_);
        }

        Function* getFunction(const IRPack& pack) const noexcept{
            if(!frame || packs[pack.i].type != __InternalIRPack__::IRPackType::FUNCTION) return nullptr;
            return &frame->funcs[packs[pack.i].index];
        }
    };
}

#endif // INERTIA_IRBUILDER_HPP
