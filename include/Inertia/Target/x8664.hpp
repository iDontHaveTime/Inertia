#ifndef INERTIA_X8664_HPP
#define INERTIA_X8664_HPP

#include "Inertia/IR/Frame.hpp"
#include "Inertia/Mem/Arenalloc.hpp"
#include "Inertia/Mem/Memstream.hpp"
#include "Inertia/Target/Target.hpp"

namespace Inertia{
    struct LowerRegisterx86_64{
        std::string_view name;
        int width; // in bits
        LowerRegisterx86_64* parent;
        LowerRegisterx86_64* child;

        LowerRegisterx86_64() noexcept : name(), width(0), parent(nullptr), child(nullptr){};
        LowerRegisterx86_64(const std::string_view& _name, int w) : name(_name), width(w), parent(nullptr), child(nullptr){};

        // returns the child
        inline LowerRegisterx86_64* set_child(LowerRegisterx86_64* c) noexcept{
            c->parent = this;
            child = c;
            return c;
        }

        // returns the parent
        inline LowerRegisterx86_64* set_parent(LowerRegisterx86_64* p) noexcept{
            p->child = this;
            parent = p;
            return p;
        }
    };

    namespace RegisterFlags{
        enum{
            CALEE_SAVED = 0x1
        };
    };

    struct GeneralRegisterx86_64{
        ArenaPointer<LowerRegisterx86_64> l64b;
        ArenaPointer<LowerRegisterx86_64> l32b;
        ArenaPointer<LowerRegisterx86_64> l16b;
        ArenaPointer<LowerRegisterx86_64> l8b;
        int flags = 0;

        GeneralRegisterx86_64() = default;
        GeneralRegisterx86_64(ArenaPointer<LowerRegisterx86_64>& b64, ArenaPointer<LowerRegisterx86_64>& b32, ArenaPointer<LowerRegisterx86_64>& b16, ArenaPointer<LowerRegisterx86_64>& b8) :
            l64b(b64), l32b(b32), l16b(b16), l8b(b8){};
    };
    struct RegisterCollectionx86_64{
        ArenaPointer<GeneralRegisterx86_64> rax;
        ArenaPointer<GeneralRegisterx86_64> rbx;
        ArenaPointer<GeneralRegisterx86_64> rcx;
        ArenaPointer<GeneralRegisterx86_64> rdx;
        ArenaPointer<GeneralRegisterx86_64> rbp;
        ArenaPointer<GeneralRegisterx86_64> rsp;
        ArenaPointer<GeneralRegisterx86_64> rsi;
        ArenaPointer<GeneralRegisterx86_64> rdi;
        ArenaPointer<GeneralRegisterx86_64> r8;
        ArenaPointer<GeneralRegisterx86_64> r9;
        ArenaPointer<GeneralRegisterx86_64> r10;
        ArenaPointer<GeneralRegisterx86_64> r11;
        ArenaPointer<GeneralRegisterx86_64> r12;
        ArenaPointer<GeneralRegisterx86_64> r13;
        ArenaPointer<GeneralRegisterx86_64> r14;
        ArenaPointer<GeneralRegisterx86_64> r15;

        ArenaPointer<GeneralRegisterx86_64> rip;

        RegisterCollectionx86_64() = default;

        inline void init(ArenaAlloc& allocator){
            allocate_general(allocator, rax, "rax", "eax", "ax", "al");

            allocate_general(allocator, rbx, "rbx", "ebx", "bx", "bl");

            allocate_general(allocator, rcx, "rcx", "ecx", "cx", "cl");

            allocate_general(allocator, rdx, "rdx", "edx", "dx", "dl");

            allocate_general(allocator, rbp, "rbp", "ebp", "bp", "bpl");

            allocate_general(allocator, rsp, "rsp", "esp", "sp", "spl");

            allocate_general(allocator, rsi, "rsi", "esi", "si", "sil");

            allocate_general(allocator, rdi, "rdi", "edi", "di", "dil");

            allocate_general(allocator, r8, "r8", "r8d", "r8w", "r8b");
            allocate_general(allocator, r9, "r9", "r9d", "r9w", "r9b");
            allocate_general(allocator, r10, "r10", "r10d", "r10w", "r10b");
            allocate_general(allocator, r11, "r11", "r11d", "r11w", "r11b");
            allocate_general(allocator, r12, "r12", "r12d", "r12w", "r13b");
            allocate_general(allocator, r13, "r13", "r13d", "r13w", "r13b");
            allocate_general(allocator, r14, "r14", "r14d", "r14w", "r14b");
            allocate_general(allocator, r15, "r15", "r15d", "r15w", "r15b");

            allocate_general(allocator, rip, "rip", "", "", "");
        }

        inline void allocate_general(ArenaAlloc& allocator, ArenaPointer<GeneralRegisterx86_64>& reg,
            const std::string_view& b64, const std::string_view& b32, const std::string_view& b16, const std::string_view& b8){
            // whoa
            reg = allocator.alloc<GeneralRegisterx86_64>();
            if(!b64.empty()) reg->l64b = allocator.alloc<LowerRegisterx86_64>(b64, 64);
            else reg->l64b = {};
            
            if(!b32.empty()) reg->l32b = allocator.alloc<LowerRegisterx86_64>(b32, 32);
            else reg->l32b = {};

            if(!b16.empty()) reg->l16b = allocator.alloc<LowerRegisterx86_64>(b16, 16);
            else reg->l16b = {};

            if(!b8.empty()) reg->l8b = allocator.alloc<LowerRegisterx86_64>(b8, 8);
            else reg->l8b = {};

            if(reg->l8b && reg->l16b){
                reg->l8b->set_parent(reg->l16b);
            }
            if(reg->l16b && reg->l32b){
                reg->l16b->set_parent(reg->l32b);
            }
            if(reg->l32b && reg->l64b){
                reg->l32b->set_parent(reg->l64b);
            }
        }
    };
    struct ABIx86_64{
        ArenaReference<GeneralRegisterx86_64> stackPointer;
        ArenaReference<GeneralRegisterx86_64> framePointer;

        void get_sysv(ArenaPointer<RegisterCollectionx86_64>& regs) noexcept{
            stackPointer = regs->rsp;
            regs->rsp->flags = 0;

            framePointer = regs->rbp;
            regs->rbp->flags |= RegisterFlags::CALEE_SAVED;
        }
    };
    struct Targetx86_64 : public TargetInfo{
        ArenaPointer<RegisterCollectionx86_64> regs;
        ABIx86_64 abi;

        Targetx86_64() = delete;

        Targetx86_64(ArenaAlloc& allocator) noexcept : TargetInfo(x86_64){
            init(allocator);
        }
        Targetx86_64(ArenaAlloc& allocator, const ABIx86_64& _abi) noexcept : TargetInfo(x86_64), abi(_abi){
            init(allocator);
        };

        bool generate_assembly(MemoryStream& mss, const Frame& frame, TargetInfo* info);

        inline void init(ArenaAlloc& allocator){
            regs = allocator.alloc<RegisterCollectionx86_64>();
            regs->init(allocator);
        }
    };
}

#endif // INERTIA_X8664_HPP
