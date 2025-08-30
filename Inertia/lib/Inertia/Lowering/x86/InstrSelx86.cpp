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

bool LowerReturnInteger(SSAConst* ssac, ArenaReference<LoweredBlock>& newBlock){
    IntegerType* it = (IntegerType*)ssac->type.get();
    int width = it->width;
    uintmax_t mask = 0;

    for(int i = 0; i < width; i++){
        mask <<= 1;
        mask |= 1;
    }

    uintmax_t value = ssac->value & mask;

    if(!value){
        newBlock->instructions.emplace_back_as<InertiaTarget::Inertiax86::Instrxor32rr>(callc.returnReg->child, callc.returnReg->child);
        return false;
    }

    return false;
}

bool LowerReturnFloat(SSAConst*, ArenaReference<LoweredBlock>&){
    return false;
}

bool LowerReturnPointer(SSAConst*, ArenaReference<LoweredBlock>&){
    return false;
}


bool LowerReturnConst(SSAConst* ssac, ArenaReference<LoweredBlock>& newBlock){
    switch(ssac->type->getKind()){
        case Type::INTEGER:
            return LowerReturnInteger(ssac, newBlock);
        case Type::FLOAT:
            return LowerReturnFloat(ssac, newBlock);
        case Type::POINTER:
            return LowerReturnPointer(ssac, newBlock);
        case Type::VOID:
            break;
    }
    return false;
}

bool LowerReturnOPx86(Insx86ctx&, ArenaReference<IRInstruction>& ins, ArenaReference<LoweredBlock>& newBlock){
    IRReturn* irret = (IRReturn*)ins.get();
    if(irret->src->ssa_type == SSAType::CONSTANT){
        if(LowerReturnConst((SSAConst*)irret->src.get(), newBlock)){
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
