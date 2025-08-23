#ifndef INERTIA_IRBUILDER_HPP
#define INERTIA_IRBUILDER_HPP

#include "Inertia/Definition/Defines.hpp"
#include "Inertia/IR/Frame.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/IR/Instruction.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include <cstddef>
#include <cstdint>

#ifdef private
#undef private
#endif

namespace Inertia{
    class __InternalIRPack__{
        size_t parent;
        enum class IRPackType{
            FUNCTION,
            BLOCK
        } type;
        size_t index;

        __InternalIRPack__() noexcept = default;
        __InternalIRPack__(IRPackType t, size_t i, size_t _parent) noexcept : parent(_parent), type(t), index(i){};

        size_t getIndex() const noexcept{
            return index;
        }
        IRPackType getType() const noexcept{
            return type;
        }

        friend class IRBuilder;
        friend class IRPack;
    };
    /* The builder returns this so that you wont access raw references */
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

        inline const Frame* getFrame() const noexcept{
            return frame;
        }
        inline const TypeAllocator* getAllocator() const noexcept{
            return talloc;
        }
        inline const ArenaAlloc* getArena() const noexcept{
            if(!talloc) return nullptr;
            return &talloc->get_arena();
        }
        inline Frame* getFrame() noexcept{
            return frame;
        }
        inline TypeAllocator* getAllocator() noexcept{
            return talloc;
        }
        inline ArenaAlloc* getArena() noexcept{
            if(!talloc) return nullptr;
            return &talloc->get_arena();
        }

    private:
        __InternalIRPack__& getInternalPack(const IRPack& pack){
            return packs[pack.i];
        }
    public:

        ArenaReference<SSAValue> newSSA(const IRPack& func, const ArenaReference<Type>& type){
            if(getInternalPack(func).getType() != __InternalIRPack__::IRPackType::FUNCTION){
                return {};
            }
            return getArena()->alloc<SSAValue>(getFunction(func)->ssaid++, type, SSAType::NORMAL);
        }

        ArenaReference<SSAValue> newSSAConst(const IRPack& func, const ArenaReference<Type>& type, inrint value){
            if(getInternalPack(func).getType() != __InternalIRPack__::IRPackType::FUNCTION){
                return {};
            }
            return (ArenaReference<SSAValue>)getArena()->alloc<SSAConst>(getFunction(func)->ssaid++, type, value);
        }

        bool buildReturn(const IRPack& block, const ArenaReference<SSAValue>& ssa){
            if(getInternalPack(block).getType() != __InternalIRPack__::IRPackType::BLOCK){
                return true;
            }
            ArenaReference<Block> blk = getBlock(block);
            blk->instructions.emplace_back_as<IRReturn>(ssa, blk, getArena());

            return false;
        }

        IRPack buildFunction(const std::string_view& name, ArenaReference<Type> type, int32_t flags = 0, uint32_t alignment = 1){
            if(!frame) return IRPack();
            size_t _func_index_ = frame->funcs.size();

            // Function& new_func =
            frame->funcs.emplace_back(name, type, &talloc->get_arena(), flags, alignment);

            size_t _pack_index_ = packs.size();
            packs.push_back({__InternalIRPack__::IRPackType::FUNCTION, _func_index_, 0});
            return IRPack(_pack_index_);
        }

        IRPack buildBlock(const std::string_view& name, const IRPack& function){
            if(!frame) return IRPack();

            Function* func = getFunction(function);
            if(!func) return IRPack();

            size_t _block_index_ = func->blocks.size();
            func->blocks.emplace_back(name, &talloc->get_arena());

            size_t _pack_index_ = packs.size();
            packs.push_back({__InternalIRPack__::IRPackType::BLOCK, _block_index_, function.getIndex()});
            return IRPack(_pack_index_);
        }

        inline Function* getFunction(const IRPack& pack) const noexcept{
            if(!frame || packs[pack.i].type != __InternalIRPack__::IRPackType::FUNCTION) return nullptr;
            return &frame->funcs[packs[pack.i].index];
        }
        inline ArenaReference<Block> getBlock(const IRPack& pack) noexcept{
            if(!frame || packs[pack.i].type != __InternalIRPack__::IRPackType::BLOCK) return {};
            return frame->funcs[packs[pack.i].parent].blocks[packs[pack.i].index];
        }

        Function* findFunction(const std::string_view& name){
            if(!frame) return nullptr;
            for(const __InternalIRPack__& pack : packs){
                if(pack.type == __InternalIRPack__::IRPackType::FUNCTION){
                    if(frame->funcs[pack.index].name == name){
                        return &frame->funcs[pack.index];
                    }
                }
            }
            return nullptr;
        }

        ~IRBuilder() noexcept = default;
    };
}

#endif // INERTIA_IRBUILDER_HPP
