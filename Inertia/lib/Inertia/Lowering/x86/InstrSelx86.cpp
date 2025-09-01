#include "Inertia/Lowering/x86/InstrSelx86.hpp"
#include "Inertia/IR/Block.hpp"
#include "Inertia/IR/Frame.hpp"
#include "Inertia/IR/Function.hpp"
#include "Inertia/IR/Instruction.hpp"
#include "Inertia/IR/Type.hpp"
#include "Inertia/Lowering/Lowered.hpp"
#include "Inertia/Lowering/LoweredOut.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include "Inertia/Target/TargetBase.hpp"
#include "Inertia/Target/x86/Targetx86.int.hpp"
#include "Inertia/Target/x86/x86CallConv.hpp"

namespace Inertia{

InertiaTarget::Inertiax86::CallingConv callc;

struct funcx86ctx{
    uint32_t alignment;
    uint32_t allocated;

    funcx86ctx() noexcept{
        allocated = 0;
        get_align();
    }

    uint32_t allocate(uint32_t size, uint32_t align) noexcept{
        uint32_t cpy = allocated;
        allocated += size;
        uint32_t misalignment = (allocated + 8) & (align - 1);
        if(misalignment != 0){
            uint32_t padding = align - misalignment;
            allocated += padding;
        }
        get_align();
        return cpy;
    }

    void get_align() noexcept{
        uint32_t ctz = __builtin_ctzl(allocated + 8);
        alignment = 1UL << ctz;
    }
};

struct Insx86ctx{
    InertiaTarget::Inertiax86::TargetBasex86* tb;
    size_t pc = 0;
    funcx86ctx fctx;

    Insx86ctx(InertiaTarget::Inertiax86::TargetBasex86* _tb) noexcept : tb(_tb){};
};

bool LowerBinaryOPx86(Insx86ctx&, ArenaReference<IRInstruction>&, ArenaReference<LoweredBlock>&){
    return false;
}

bool LowerLoadOPx86(Insx86ctx&, ArenaReference<IRInstruction>&, ArenaReference<LoweredBlock>&){
    return false;
}

bool LowerStoreOPx86(Insx86ctx&, ArenaReference<IRInstruction>&, ArenaReference<LoweredBlock>&){
    return false;
}

bool LowerAllocOPx86(Insx86ctx&, ArenaReference<IRInstruction>&, ArenaReference<LoweredBlock>&){
    return false;
}

bool XorRegRegx86(InertiaTarget::RegisterBase* rbase, ArenaReference<LoweredBlock>& newBlock){
    if(rbase->width == 64 || rbase->width == 32){
        if(rbase->width == 64){
            if(!rbase->child){
                newBlock->instructions.emplace_back_as<InertiaTarget::Inertiax86::Instrxor64rr>(rbase, rbase);
                return false;
            }
            else{
                rbase = rbase->child;
            }
            newBlock->instructions.emplace_back_as<InertiaTarget::Inertiax86::Instrxor32rr>(rbase, rbase);
        }
        else if(rbase->width == 16){
            newBlock->instructions.emplace_back_as<InertiaTarget::Inertiax86::Instrxor16rr>(rbase, rbase);
        }
        else if(rbase->width == 8){
            newBlock->instructions.emplace_back_as<InertiaTarget::Inertiax86::Instrxor8rr>(rbase, rbase);
        }
        else{
            return true;
        }
    }
    return false;
}

bool MoveImmediateToGPR(InertiaTarget::RegisterBase* reg, inrint value, ArenaReference<LoweredBlock>& newBlock, bool full, int width){
    if(!value){
        if(!full){
            InertiaTarget::RegisterBase* rbase = reg;
            while(width < rbase->width){
                if(!rbase->child){
                    break;
                }
                rbase = rbase->child;
            }
            XorRegRegx86(rbase, newBlock);
        }
        else{
            XorRegRegx86(reg, newBlock);
        }
        return false;
    }

    InertiaTarget::RegisterBase* rbase = reg;
    if(!full){
        while(width < rbase->width){
            if(!rbase->child){
                break;
            }
            rbase = rbase->child;
        }
    }

    switch(rbase->width){
        case 64:{
                intmax_t sval = (intmax_t)value;
                if(sval >= INT32_MIN && sval <= INT32_MAX){
                    newBlock->instructions.emplace_back_as<InertiaTarget::Inertiax86::Instrmov64i32r>(rbase, sval);
                }
                else{
                    newBlock->instructions.emplace_back_as<InertiaTarget::Inertiax86::Instrmov64i64r>(rbase, value);
                }
            }
            break;
        case 32:
            newBlock->instructions.emplace_back_as<InertiaTarget::Inertiax86::Instrmov32i32r>(rbase, (uint32_t)value);
            break;
        case 16:
            newBlock->instructions.emplace_back_as<InertiaTarget::Inertiax86::Instrmov16i16r>(rbase, (uint16_t)value);
            break;
        case 8:
            newBlock->instructions.emplace_back_as<InertiaTarget::Inertiax86::Instrmov8i8r>(rbase, (uint8_t)value);
            break;
        default:
            return true;
    }

    return false;
}

bool LowerReturnInteger(SSAConst* ssac, ArenaReference<LoweredBlock>& newBlock, int width){
    uintmax_t mask = (width >= (int)(sizeof(uintmax_t) * 8)) ? ~0ull : (1ull << width) - 1;
    uintmax_t truncated = ssac->value & mask;

    return MoveImmediateToGPR(callc.returnReg, truncated, newBlock, true, width);
}

bool LowerReturnFloat(SSAConst*, ArenaReference<LoweredBlock>&){
    return false;
}

bool LowerReturnPointer(Insx86ctx& ctx, SSAConst* ssac, ArenaReference<LoweredBlock>& newBlock){
    return LowerReturnInteger(ssac, newBlock, ctx.tb->ptr_size);
}

bool LowerReturnConst(Insx86ctx& ctx, SSAConst* ssac, ArenaReference<LoweredBlock>& newBlock){
    switch(ssac->type->getKind()){
        case Type::INTEGER:
            return LowerReturnInteger(ssac, newBlock, ((IntegerType*)ssac->type.get())->width);
        case Type::FLOAT:
            return LowerReturnFloat(ssac, newBlock);
        case Type::POINTER:
            return LowerReturnPointer(ctx, ssac, newBlock);
        case Type::VOID:
            break;
    }
    return false;
}

bool LowerReturnOPx86(Insx86ctx& ctx, ArenaReference<IRInstruction>& ins, ArenaReference<LoweredBlock>& newBlock){
    IRReturn* irret = (IRReturn*)ins.get();
    if(irret->src->ssa_type == SSAType::CONSTANT){
        if(LowerReturnConst(ctx, (SSAConst*)irret->src.get(), newBlock)){
            return true;
        }
    }
    else{

    }
    newBlock->instructions.emplace_back_as<InertiaTarget::Inertiax86::Instrret>();
    return false;
}

bool LowerInstructionx86(Insx86ctx& ctx, ArenaReference<IRInstruction>& ins, ArenaReference<LoweredBlock>& newBlock){
    switch(ins->op){
        case IROpType::Unknown:
            return true;
        case IROpType::Add:
            [[fallthrough]];
        case IROpType::Sub:
            [[fallthrough]];
        case IROpType::Mul:
            [[fallthrough]];
        case IROpType::Div:
            return LowerBinaryOPx86(ctx, ins, newBlock);
        case IROpType::Load:
            return LowerLoadOPx86(ctx, ins, newBlock);
        case IROpType::Store:
            return LowerStoreOPx86(ctx, ins, newBlock);
        case IROpType::Ret:
            return LowerReturnOPx86(ctx, ins, newBlock);
        case IROpType::Alloc:
            return LowerAllocOPx86(ctx, ins, newBlock);
    }
    return false;
}

bool LowerBlockx86(Insx86ctx& ctx, ArenaReference<Block>& block, LoweredFunction& newFunc){
    ArenaReference<LoweredBlock> newBlock = newFunc.blocks.emplace_back(block, block->instructions.get_arena());
    bool done = false;
    for(ArenaReference<IRInstruction> ins : block->instructions){
        if(!done){
            if(LowerInstructionx86(ctx, ins, newBlock)){
                return true;
            }
        }
        ctx.pc++;
        if((uint16_t)ins->op & TERM){
            done = true;
        }
    }
    return false;
}

bool LowerFunctionx86(Insx86ctx ctx, LoweredOutput& to, Function& func){
    LoweredFunction& newFunc = to.funcs.emplace_back(&func, func.blocks.get_arena());
    callc.get_cc(to.ttriple->getLoadedABI(), ctx.tb);
    ctx.fctx = {};
    for(ArenaReference<Block>& block : func.blocks){
        if(LowerBlockx86(ctx, block, newFunc)){
            return true;
        }
    }
    return false;
}

bool InstructionSelectorx86::lower(Frame& frame, LoweredOutput& to, InertiaTarget::TargetBase* tb){
    if(!to) return true;
    reserve_lower(frame, to);

    Insx86ctx ctx((InertiaTarget::Inertiax86::TargetBasex86*)tb);

    for(Function& func : frame.funcs){
        if(LowerFunctionx86(ctx, to, func)){
            return true;
        }
    }

    return false;
}

}
