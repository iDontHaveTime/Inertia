// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_INSTDEF_H
#define INERTIA_IR_INSTDEF_H

/// @file IR/InstDef.h
/// @brief Provides the instruction class and the instructions.

#include <inr/ADT/ArrView.h>
#include <inr/ADT/IList.h>
#include <inr/IR/Def.h>
#include <inr/IR/Type.h>
#include <inr/IR/TypeMap.h>
#include <inr/IR/UseDef.h>
#include <inr/Support/Assert.h>

namespace inr {

class BlockDef;

/// @brief Base class for all instructions.
/// @note Use the static methods to create new instructions. NEVER use delete on
/// them manually.
///
/// Uses the UseDef's uses vector as its operands.
class InstDef : public UseDef, public ilist_node<InstDef> {
public:
    /// @brief Every possible instruction type in the IR.
    enum InstType : unsigned {
        /// @brief Returns to the caller of the function.
        ///
        /// For example:
        /// ```llvm
        /// ret i32 0
        /// ```
        Ret,
        /// @brief Jumps to another block in the function.
        /// @note Can be both conditional and non-conditional.
        ///
        /// For example, conditional:
        /// ```llvm
        /// jmp i1 %cond, block %iftrue, block %iffalse
        /// ```
        /// Non conditional example:
        /// ```llvm
        /// jmp block %some_block
        /// ```
        Jmp,
        /// @brief Terminates this block without returning or jumping anywhere.
        /// @note Just marked as `unreachable` no need for an "example".
        Unreachable,
        /// @brief Compares two values together.
        ///
        /// For example:
        /// ```llvm
        /// %res = i32 cmp.eq(%val1, %val2)
        /// ```
        /// In the example above `i32` means the type of the args, not the
        /// result type.
        /// The result type is always `i1`.
        /// Possible comparisons:
        /// - `eq` - Two values are equal.
        /// - `neq` - Two values are not equal.
        /// - `ug` - Lhs is unsigned-wise bigger than the rhs.
        /// - `uge` - Lhs is unsigned-wise bigger or equal to the rhs.
        /// - `ul` - Lhs is unsigned-wise lesser than the rhs.
        /// - `ule` - Lhs is unsigned-wise lesser or equal to the rhs.
        /// - `sg` - Lhs is signed-wise greater than the rhs.
        /// - `sge` - Lhs is signed-wise greater or equal to the rhs.
        /// - `sl` - Lhs is signed-wise lesser than the rhs.
        /// - `sle` - Lhs is signed-wise lesser or equal to the rhs.
        Cmp,
        /// @brief Accepts values based on the block it came from.
        ///
        /// For example:
        /// ```llvm
        /// %x0 = i32 phi([%x1, %y0], [%x2, %y1])
        /// ```
        /// The example above means basically that `%x0` will be `%x1` if we
        /// came from block `%y0` and if we came from block `%y1` it would be
        /// `%x2`.
        Phi,
        /// @brief Adds two values together.
        ///
        /// For example:
        /// ```llvm
        /// %x0 = i32 add(%x1, 42)
        /// ```
        Add,
        /// @brief Subtracts two values.
        ///
        /// For example:
        /// ```llvm
        /// %x0 = i32 sub(%x1, 42)
        /// ```
        Sub,
        /// @brief Multiplies two values.
        ///
        /// For example:
        /// ```llvm
        /// %x0 = i32 mul(%x1, 42)
        /// ```
        Mul,
        /// @brief Unsigned division of two values.
        ///
        /// For example:
        /// ```llvm
        /// %x0 = i32 udiv(%x1, 42)
        /// ```
        UDiv,
        /// @brief Signed division of two values.
        ///
        /// For example:
        /// ```llvm
        /// %x0 = i32 sdiv(%x1, 42)
        /// ```
        SDiv,
        /// @brief Unsigned remainder (modulo) of two values.
        ///
        /// For example:
        /// ```llvm
        /// %x0 = i32 urem(%x1, 42)
        /// ```
        URem,
        /// @brief Signed remainder (modulo) of two values.
        ///
        /// For example:
        /// ```llvm
        /// %x0 = i32 srem(%x1, 42)
        /// ```
        SRem,
        /// @brief Shift lhs left by rhs.
        ///
        /// For example:
        /// ```llvm
        /// %x0 = i32 shl(%x1, 42)
        /// ```
        Shl,
        /// @brief Shift lhs right by rhs, filling it with zeros.
        ///
        /// For example:
        /// ```llvm
        /// %x0 = i32 lshr(%x1, 42)
        /// ```
        LShr,
        /// @brief Shift lhs right by rhs, filling it with the sign bit.
        ///
        /// For example:
        /// ```llvm
        /// %x0 = i32 ashr(%x1, 42)
        /// ```
        AShr,
        /// @brief Performs bitwise AND.
        ///
        /// For example:
        /// ```llvm
        /// %x0 = i32 and(%x1, 42)
        /// ```
        And,
        /// @brief Performs bitwise OR.
        ///
        /// For example:
        /// ```llvm
        /// %x0 = i32 or(%x1, 42)
        /// ```
        Or,
        /// @brief Performs bitwise XOR.
        ///
        /// For example:
        /// ```llvm
        /// %x0 = i32 xor(%x1, 42)
        /// ```
        Xor,
        /// @brief Loads from the provided pointer.
        ///
        /// For example:
        /// ```llvm
        /// %x0 = i32 load(%x1)
        /// ```
        Load,
        /// @brief Stores into the provided pointer.
        ///
        /// For example:
        /// ```llvm
        /// store %x0, i32 %x1
        /// ```
        Store,
        /// @brief Allocates space on stack.
        ///
        /// For example:
        /// ```llvm
        /// %x0 = alloca(i32, i32 1)
        /// ```
        Alloca,
    };

private:
    InstType instType_;

protected:
    InstDef(const Type* type, std::string_view name, InstType instType) :
        UseDef(type, InstDefType, name), instType_(instType) {}

public:
    /// @brief Returns the instruction kind of this instruction.
    InstType getInstType() const {
        return instType_;
    }

    /// @brief Returns true if this instruction terminates the block.
    bool isTerminator() const {
        switch(instType_) {
            case Ret:
            case Jmp:
            case Unreachable:
                return true;
            default:
                return false;
        }
    }
};

/// @brief Represents the `ret` instruction.
class RetInst : public InstDef {
    RetInst(const Type* type, Def* retVal) : InstDef(type, {}, Ret) {
        addUse(retVal);
    }

    RetInst(const Type* type) : InstDef(type, {}, Ret) {}

public:
    /// @brief Creates a new return instruction.
    /// @param blk Block to append it to.
    /// @param retVal Return value, nullptr for void.
    static RetInst* createRet(TypeMap& tm, BlockDef* blk, Def* retVal);
    /// @brief Creates a new return instruction without a return value.
    /// @param blk Block to append it to.
    static RetInst* createRetVoid(TypeMap& tm, BlockDef* blk);

    /// @brief Returns true if this return instruction does not return a value.
    bool isRetVoid() const {
        return getType()->isVoid();
    }

    /// @brief Returns the return value of this instruction.
    /// @note Unsafe if `isRetVoid()` is true.
    Def* getRetVal() {
        return getUses()[0];
    }

    /// @brief Returns the return value of this instruction, const version.
    /// @note Unsafe if `isRetVoid()` is true.
    const Def* getRetVal() const {
        return getUses()[0];
    }
};

/// @brief Represents the `jmp` instruction.
class JmpInst : public InstDef {
    JmpInst(const VoidType* type, Def* lbl) : InstDef(type, {}, Jmp) {
        addUse(lbl);
    }

    JmpInst(const VoidType* type, Def* cond, Def* iftrue, Def* iffalse) :
        InstDef(type, {}, Jmp) {
        addUse(cond);
        addUse(iftrue);
        addUse(iffalse);
    }

public:
    /// @brief Creates a new non conditional jump.
    /// @param blk Block to append it to.
    /// @param lbl Block it should jump to.
    static JmpInst* createJmp(TypeMap& tm, BlockDef* blk, Def* lbl);
    /// @brief Creates a new conditional jump.
    /// @param blk Block to append it to.
    /// @param cond An i1 condition def.
    /// @param iftrue Block to jump to if the condition is 1.
    /// @param iffalse Block to jump to if the condition is 0.
    static JmpInst* createJmpCond(TypeMap& tm, BlockDef* blk, Def* cond,
                                  Def* iftrue, Def* iffalse);

    /// @brief Returns true if this jump is conditional.
    bool isConditional() const {
        return getUses().size() == 3;
    }

    /// @brief Returns true if this jump is non conditional.
    bool isNonConditional() const {
        return getUses().size() == 1;
    }

    /// @brief Returns the condition of this jump.
    /// @note Unsafe if not conditional.
    Def* getCondition() {
        return getUses()[0];
    }

    /// @brief Returns the block it jumps to if the condition is true.
    /// @note Unsafe if not conditional.
    Def* getIfTrue() {
        return getUses()[1];
    }

    /// @brief Returns the block it jumps to if the condition is false.
    /// @note Unsafe if not conditional.
    Def* getIfFalse() {
        return getUses()[2];
    }

    /// @brief Returns the condition of this jump, const version.
    /// @note Unsafe if not conditional.
    const Def* getCondition() const {
        return getUses()[0];
    }

    /// @brief Returns the block it jumps to if the condition is true, const
    /// version.
    /// @note Unsafe if not conditional.
    const Def* getIfTrue() const {
        return getUses()[1];
    }

    /// @brief Returns the block it jumps to if the condition is false, const
    /// version.
    /// @note Unsafe if not conditional.
    const Def* getIfFalse() const {
        return getUses()[2];
    }

    /// @brief Returns the block it jumps to.
    /// @note Unsafe if conditional.
    Def* getNonCondBlock() {
        return getUses()[0];
    }

    /// @brief Returns the block it jumps to, const version.
    /// @note Unsafe if conditional.
    const Def* getNonCondBlock() const {
        return getUses()[0];
    }
};

/// @brief Represents a binary instruction.
/// @note Should not be used, use the derived classes instead.
class BinaryInst : public InstDef {
protected:
    BinaryInst(const Type* type, std::string_view name, InstType instType,
               Def* lhs, Def* rhs) :
        InstDef(type, name, instType) {
        addUse(lhs);
        addUse(rhs);
    }

public:
    /// @brief Returns the left hand side of this binary instruction.
    Def* getLhs() {
        return getUses()[0];
    }

    /// @brief Returns the right hand side of this binary instruction.
    Def* getRhs() {
        return getUses()[1];
    }

    /// @brief Returns the left hand side of this binary instruction, const
    /// version.
    const Def* getLhs() const {
        return getUses()[0];
    }

    /// @brief Returns the right hand side of this binary instruction, const
    /// version.
    const Def* getRhs() const {
        return getUses()[1];
    }
};

/// @brief Represents the `cmp` instruction.
/// @note This is an integer comparison instruction.
class CmpInst : public BinaryInst {
public:
    /// @brief Explained under the `InstDef::Cmp` enum.
    enum CmpCond : unsigned {
        Equal,         ///< Equal.
        NotEqual,      ///< Not equal.
        UGreater,      ///< Unsigned greater.
        UGreaterEqual, ///< Unsigned greater or equal.
        ULess,         ///< Unsigned less.
        ULessEqual,    ///< Unsigned less or equal.
        SGreater,      ///< Signed greater.
        SGreaterEqual, ///< Signed greater or equal.
        SLess,         ///< Signed less.
        SLessEqual,    ///< Signed less or equal.
    };

private:
    CmpCond cond_;

    CmpInst(const Type* type, std::string_view name, CmpCond cond, Def* lhs,
            Def* rhs) :
        BinaryInst(type, name, Cmp, lhs, rhs), cond_(cond) {}

public:
    /// @brief Returns the condition of this comparison.
    CmpCond getCond() const {
        return cond_;
    }

    /// @brief Creates a new comparison instruction.
    /// @param blk Block to append it to.
    /// @param cond Condition for this comparison.
    /// @param lhs Left hand side for this comparison.
    /// @param rhs Right hand side for this comparison.
    /// @param name Name for the result.
    static CmpInst* createCmp(TypeMap& tm, BlockDef* blk, CmpCond cond,
                              Def* lhs, Def* rhs, std::string_view name = {});
};

/// @brief Represents the `mul` instruction.
class MulInst : public BinaryInst {
    MulInst(const Type* type, std::string_view name, Def* lhs, Def* rhs) :
        BinaryInst(type, name, Mul, lhs, rhs) {}

public:
    /// @brief Creates a new mul instruction.
    /// @param blk Block to append it to.
    /// @param lhs Left hand side for this instruction.
    /// @param rhs Right hand side for this instruction.
    /// @param name Name for the result.
    static MulInst* createMul(BlockDef* blk, Def* lhs, Def* rhs,
                              std::string_view name = {});
};

/// @brief Represents the `udiv` instruction.
class UDivInst : public BinaryInst {
    UDivInst(const Type* type, std::string_view name, Def* lhs, Def* rhs) :
        BinaryInst(type, name, UDiv, lhs, rhs) {}

public:
    /// @brief Creates a new udiv instruction.
    /// @param blk Block to append it to.
    /// @param lhs Left hand side for this instruction.
    /// @param rhs Right hand side for this instruction.
    /// @param name Name for the result.
    static UDivInst* createUDiv(BlockDef* blk, Def* lhs, Def* rhs,
                                std::string_view name = {});
};

/// @brief Represents the `sdiv` instruction.
class SDivInst : public BinaryInst {
    SDivInst(const Type* type, std::string_view name, Def* lhs, Def* rhs) :
        BinaryInst(type, name, SDiv, lhs, rhs) {}

public:
    /// @brief Creates a new sdiv instruction.
    /// @param blk Block to append it to.
    /// @param lhs Left hand side for this instruction.
    /// @param rhs Right hand side for this instruction.
    /// @param name Name for the result.
    static SDivInst* createSDiv(BlockDef* blk, Def* lhs, Def* rhs,
                                std::string_view name = {});
};

/// @brief Represents the `urem` instruction.
class URemInst : public BinaryInst {
    URemInst(const Type* type, std::string_view name, Def* lhs, Def* rhs) :
        BinaryInst(type, name, URem, lhs, rhs) {}

public:
    /// @brief Creates a new urem instruction.
    /// @param blk Block to append it to.
    /// @param lhs Left hand side for this instruction.
    /// @param rhs Right hand side for this instruction.
    /// @param name Name for the result.
    static URemInst* createURem(BlockDef* blk, Def* lhs, Def* rhs,
                                std::string_view name = {});
};

/// @brief Represents the `srem` instruction.
class SRemInst : public BinaryInst {
    SRemInst(const Type* type, std::string_view name, Def* lhs, Def* rhs) :
        BinaryInst(type, name, SRem, lhs, rhs) {}

public:
    /// @brief Creates a new srem instruction.
    /// @param blk Block to append it to.
    /// @param lhs Left hand side for this instruction.
    /// @param rhs Right hand side for this instruction.
    /// @param name Name for the result.
    static SRemInst* createSRem(BlockDef* blk, Def* lhs, Def* rhs,
                                std::string_view name = {});
};

/// @brief Represents the `add` instruction.
class AddInst : public BinaryInst {
    AddInst(const Type* type, std::string_view name, Def* lhs, Def* rhs) :
        BinaryInst(type, name, Add, lhs, rhs) {}

public:
    /// @brief Creates a new add instruction.
    /// @param blk Block to append it to.
    /// @param lhs Left hand side for this instruction.
    /// @param rhs Right hand side for this instruction.
    /// @param name Name for the result.
    static AddInst* createAdd(BlockDef* blk, Def* lhs, Def* rhs,
                              std::string_view name = {});
};

/// @brief Represents the `sub` instruction.
class SubInst : public BinaryInst {
    SubInst(const Type* type, std::string_view name, Def* lhs, Def* rhs) :
        BinaryInst(type, name, Sub, lhs, rhs) {}

public:
    /// @brief Creates a new sub instruction.
    /// @param blk Block to append it to.
    /// @param lhs Left hand side for this instruction.
    /// @param rhs Right hand side for this instruction.
    /// @param name Name for the result.
    static SubInst* createSub(BlockDef* blk, Def* lhs, Def* rhs,
                              std::string_view name = {});
};

/// @brief Represents the `shl` instruction.
class ShlInst : public BinaryInst {
    ShlInst(const Type* type, std::string_view name, Def* lhs, Def* rhs) :
        BinaryInst(type, name, Shl, lhs, rhs) {}

public:
    /// @brief Creates a new shl instruction.
    /// @param blk Block to append it to.
    /// @param lhs Left hand side for this instruction.
    /// @param rhs Right hand side for this instruction.
    /// @param name Name for the result.
    static ShlInst* createShl(BlockDef* blk, Def* lhs, Def* rhs,
                              std::string_view name = {});
};

/// @brief Represents the `lshr` instruction.
class LShrInst : public BinaryInst {
    LShrInst(const Type* type, std::string_view name, Def* lhs, Def* rhs) :
        BinaryInst(type, name, LShr, lhs, rhs) {}

public:
    /// @brief Creates a new lshr instruction.
    /// @param blk Block to append it to.
    /// @param lhs Left hand side for this instruction.
    /// @param rhs Right hand side for this instruction.
    /// @param name Name for the result.
    static LShrInst* createLShr(BlockDef* blk, Def* lhs, Def* rhs,
                                std::string_view name = {});
};

/// @brief Represents the `ashr` instruction.
class AShrInst : public BinaryInst {
    AShrInst(const Type* type, std::string_view name, Def* lhs, Def* rhs) :
        BinaryInst(type, name, AShr, lhs, rhs) {}

public:
    /// @brief Creates a new ashr instruction.
    /// @param blk Block to append it to.
    /// @param lhs Left hand side for this instruction.
    /// @param rhs Right hand side for this instruction.
    /// @param name Name for the result.
    static AShrInst* createAShr(BlockDef* blk, Def* lhs, Def* rhs,
                                std::string_view name = {});
};

/// @brief Represents the `and` instruction.
class AndInst : public BinaryInst {
    AndInst(const Type* type, std::string_view name, Def* lhs, Def* rhs) :
        BinaryInst(type, name, And, lhs, rhs) {}

public:
    /// @brief Creates a new and instruction.
    /// @param blk Block to append it to.
    /// @param lhs Left hand side for this instruction.
    /// @param rhs Right hand side for this instruction.
    /// @param name Name for the result.
    static AndInst* createAnd(BlockDef* blk, Def* lhs, Def* rhs,
                              std::string_view name = {});
};

/// @brief Represents the `or` instruction.
class OrInst : public BinaryInst {
    OrInst(const Type* type, std::string_view name, Def* lhs, Def* rhs) :
        BinaryInst(type, name, Or, lhs, rhs) {}

public:
    /// @brief Creates a new or instruction.
    /// @param blk Block to append it to.
    /// @param lhs Left hand side for this instruction.
    /// @param rhs Right hand side for this instruction.
    /// @param name Name for the result.
    static OrInst* createOr(BlockDef* blk, Def* lhs, Def* rhs,
                            std::string_view name = {});
};

/// @brief Represents the `xor` instruction.
class XorInst : public BinaryInst {
    XorInst(const Type* type, std::string_view name, Def* lhs, Def* rhs) :
        BinaryInst(type, name, Xor, lhs, rhs) {}

public:
    /// @brief Creates a new xor instruction.
    /// @param blk Block to append it to.
    /// @param lhs Left hand side for this instruction.
    /// @param rhs Right hand side for this instruction.
    /// @param name Name for the result.
    static XorInst* createXor(BlockDef* blk, Def* lhs, Def* rhs,
                              std::string_view name = {});
};

/// @brief Represents the `phi` instruction.
class PhiInst : public InstDef {
    ivec<BlockDef*, 4> block_;

    PhiInst(const Type* type, std::string_view name) :
        InstDef(type, name, Phi) {}

public:
    /// @brief Creates a new phi instruction.
    /// @param blk Block to append it to.
    /// @param type Type for the value.
    /// @param name Name for the result.
    /// @note To add incoming values use `addIncoming()`.
    static PhiInst* createPhi(BlockDef* blk, const Type* type,
                              std::string_view name = {});

    arrview<BlockDef*> getBlocks() const {
        return block_;
    }

    /// @brief Adds an incoming value for this phi node.
    /// @param def The def to accept.
    /// @param blk The block it comes from.
    void addIncoming(Def* def, BlockDef* blk) {
        inr_assert(def != nullptr,
                   "PhiInst addIncoming(): passed in a nullptr def");
        inr_assert(blk != nullptr,
                   "PhiInst addIncoming(): passed in a nullprt block");
        addUse(def);
        block_.emplace_back(blk);
    }

    /// @brief Returns the def and block of an incoming value.
    std::pair<Def*, BlockDef*> getIncoming(unsigned i) {
        return {getUses()[i], block_[i]};
    }

    /// @brief Returns the def and block of an incoming value, const version.
    std::pair<const Def*, const BlockDef*> getIncoming(unsigned i) const {
        return {getUses()[i], block_[i]};
    }

    unsigned getIncomingCount() const {
        return getUses().size();
    }
};

/// @brief Represents the `unreachable` instruction.
class UnreachableInst : public InstDef {
    UnreachableInst(const VoidType* vt) : InstDef(vt, {}, Unreachable) {}

public:
    /// @brief Creates a new unreachable instruction.
    /// @param blk Block to append it to.
    static UnreachableInst* createUnreachable(TypeMap& tm, BlockDef* blk);
};

/// @brief Represents the `load` instruction.
class LoadInst : public InstDef {
    LoadInst(const Type* type, std::string_view name, Def* from) :
        InstDef(type, name, Load) {
        addUse(from);
    }

public:
    /// @brief Creates a new load instruction.
    /// @param blk Block to append it to.
    /// @param type The type to load from the pointer.
    /// @param from Pointer to load it from.
    /// @param name Name for the result.
    static LoadInst* createLoad(BlockDef* blk, const Type* type, Def* from,
                                std::string_view name = {});

    /// @brief Returns the pointer it loads from.
    Def* getFrom() {
        return getUses()[0];
    }

    /// @brief Returns the pointer it loads from, const version.
    const Def* getFrom() const {
        return getUses()[0];
    }
};

/// @brief Represents the `store` instruction.
class StoreInst : public InstDef {
    StoreInst(const VoidType* vt, Def* to, Def* from) : InstDef(vt, {}, Store) {
        addUse(to);
        addUse(from);
    }

public:
    /// @brief Creates a new store instruction.
    /// @param blk Block to append it to.
    /// @param to Pointer to store it to.
    /// @param from The value to store.
    static StoreInst* createStore(TypeMap& tm, BlockDef* blk, Def* to,
                                  Def* from);

    /// @brief Returns the def it stores into.
    Def* getTo() {
        return getUses()[0];
    }

    /// @brief Returns the def it uses as the value.
    Def* getFrom() {
        return getUses()[1];
    }

    /// @brief Returns the def it stores into, const version.
    const Def* getTo() const {
        return getUses()[0];
    }

    /// @brief Returns the def it uses as the value, const version.
    const Def* getFrom() const {
        return getUses()[1];
    }
};

/// @brief Represents the `alloca` instruction.
class AllocaInst : public InstDef {
    const Type* allocates_;
    AllocaInst(const PtrType* ptr, std::string_view name, const Type* t,
               Def* count) :
        InstDef(ptr, name, Alloca), allocates_(t) {
        addUse(count);
    }

public:
    /// @brief The type that alloca allocates.
    const Type* getAllocaType() const {
        return allocates_;
    }

    /// @brief Returns the amount of slots to allocate.
    Def* getCount() {
        return getUses()[0];
    }

    /// @brief Returns the amount of slots to allocate, const version.
    const Def* getCount() const {
        return getUses()[0];
    }

    /// @brief Creates a new alloca instruction.
    /// @param blk Block to append it to.
    /// @param toAllocate The type it should allocate.
    /// @param count The amount of slots to allocate for the type.
    /// @param name Name for the result.
    static AllocaInst* createAlloca(TypeMap& tm, BlockDef* blk,
                                    const Type* toAllocate, Def* count,
                                    std::string_view name = {});
};

} // namespace inr

#endif // INERTIA_IR_INSTDEF_H
