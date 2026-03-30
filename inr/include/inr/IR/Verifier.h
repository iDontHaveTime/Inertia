// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_VERIFIER_H
#define INERTIA_IR_VERIFIER_H

/// @file IR/Verifier.h
/// @brief Provides a function to verify modules.

#include <inr/IR/Block.h>
#include <inr/IR/Function.h>
#include <inr/IR/Instruction.h>
#include <inr/IR/Module.h>
#include <inr/Support/Stream.h>

#include <vector>

namespace inr {

/// @brief Base class for module errors.
class ModuleError {
public:
    enum class Kind {
        Module,     ///< Error relating the module.
        Function,   ///< Error relating the function.
        Block,      ///< Error relating the block.
        Instruction ///< Error relating the instruction.
    };

private:
    Kind kind_;

public:
    ModuleError(Kind kind) noexcept : kind_(kind) {}

    /// @brief Returns the kind of the error.
    Kind getKind() const noexcept {
        return kind_;
    }

    /// @brief Prints out the error with a descriptive message of it.
    virtual void strerr(raw_stream&) const = 0;

    virtual ~ModuleError() noexcept = default;
};

class ModuleErrors final {
    std::vector<ModuleError*> errors_;

    void addError(ModuleError* err) {
        errors_.emplace_back(err);
    }

    ModuleErrors() noexcept = default;

public:
    ModuleErrors(const ModuleErrors&) = delete;
    ModuleErrors& operator=(const ModuleErrors&) = delete;

    ModuleErrors(ModuleErrors&&) = default;
    ModuleErrors& operator=(ModuleErrors&&) = default;

    /// @brief Returns whether or not errors are present.
    bool hasErrors() const noexcept {
        return errors_.size();
    }

    const std::vector<ModuleError*>& getErrors() const noexcept {
        return errors_;
    }

    /// @brief Prints out all errors in order.
    void printAll(raw_stream& os) const {
        for(ModuleError* err : errors_) {
            err->strerr(os);
        }
    }

    ~ModuleErrors() noexcept {
        for(auto it = errors_.rbegin(); it != errors_.rend(); ++it) {
            delete *it;
        }
    }

    /// @brief Implementation for the verifyModule(Module*) function.
    friend class ModuleVerifier;
};

/// @brief Verifies the moduels and returns whether or not errors were present.
/// @param module The module to verify.
ModuleErrors verifyModule(const Module* module);

class FunctionError : public ModuleError {
public:
    enum class SubKind {
        Redefinition,        ///< A function with the same name already exists.
        MismatchedSignature, ///< The args don't match the signature.
        NoEntryBlock         ///< This function has no blocks present.
    };

private:
    const Function* function_;
    SubKind subkind_;

public:
    FunctionError(const Function* function, SubKind subkind) noexcept :
        ModuleError(Kind::Function), function_(function), subkind_(subkind) {}

    void strerr(raw_stream&) const override;

    SubKind getSubKind() const noexcept {
        return subkind_;
    }

    const Function* getFunction() const noexcept {
        return function_;
    }
};

class BlockError : public ModuleError {
public:
    enum class SubKind {
        Redefinition,       ///< A block with the same name already exists.
        NoTerminator,       ///< This block has no terminator.
        TerminatorIsntLast, ///< This block's terminator isn't the last
                            ///< instruction.
        MultipleTerminators ///< This block has multiple terminators.
    };

private:
    const Block* block_;
    SubKind subkind_;

public:
    BlockError(const Block* block, SubKind subkind) noexcept :
        ModuleError(Kind::Block), block_(block), subkind_(subkind) {}

    void strerr(raw_stream&) const override;

    SubKind getSubKind() const noexcept {
        return subkind_;
    }

    const Block* getBlock() const noexcept {
        return block_;
    }
};

class InstructionError : public ModuleError {
public:
    enum class SubKind {
        ReturnTypeMismatch, ///< Return type doesn't match function's return
                            ///< type.
        ReturnNoOperand,    ///< Return has a non-void type but doesn't return a
                            ///< value.
        OperandTypeMismatch, ///< Operands of this instruction don't match.
        TooManyOperands,     ///< This instruction has too many operands.
        TooLittleOperands, ///< This instruction has less operands than needed.
        /// @brief Provided type is not allowed in instructions.
        ///
        /// For example: %x = add void 1, 2; You cannot add void.
        TypeNotAllowed
    };

private:
    const Instruction* inst_;
    SubKind subkind_;

public:
    InstructionError(const Instruction* inst, SubKind subkind) noexcept :
        ModuleError(Kind::Instruction), inst_(inst), subkind_(subkind) {}

    void strerr(raw_stream&) const override;

    SubKind getSubKind() const noexcept {
        return subkind_;
    }

    const Instruction* getInstruction() const noexcept {
        return inst_;
    }
};

} // namespace inr

#endif // INERTIA_IR_VERIFIER_H
