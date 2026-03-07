#ifndef INERTIA_IR_FUNCTION_H
#define INERTIA_IR_FUNCTION_H

/// @file IR/Function.h
/// @brief Contains the function class.

#include <inr/ADT/IList.h>
#include <inr/ADT/StrView.h>
#include <inr/IR/Argument.h>
#include <inr/IR/Block.h>
#include <inr/IR/Type.h>
#include <inr/IR/Value.h>

namespace inr {

class Function : public Value, public ilist_node<Function> {
    ilist<Block> blocks_;
    std::vector<Argument> args_;

    Function(const FunctionType* signature, sview name) noexcept :
        Value(ValueID::Function, signature, name) {
        for(size_t i = 0; i < signature->getArgs().size(); i++) {
            args_.emplace_back(this, signature->getArgs()[i], i);
        }
    }

public:
    const FunctionType* getType() const noexcept {
        return (const FunctionType*)type_;
    }

    const std::vector<Argument>& getArgs() const noexcept {
        return args_;
    }

    void setArgName(unsigned arg, sview name) {
        // vector<T>::at() checks for bounds.
        args_.at(arg).setName(name);
    }

    Argument* getArg(unsigned arg) noexcept {
        return args_.data() + arg;
    }

    const Argument* getArg(unsigned arg) const noexcept {
        return args_.data() + arg;
    }

    Block* newBlock(sview name) {
        return blocks_.push_back(new Block(this, name));
    }

    ~Function() noexcept {
        for(Block* b = blocks_.head; b != nullptr;) {
            Block* next = b->next;
            delete b;
            b = next;
        }
    }

    friend class Module;
};

} // namespace inr

#endif // INERTIA_IR_FUNCTION_H
