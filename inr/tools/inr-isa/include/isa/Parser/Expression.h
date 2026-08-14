// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_PARSER_EXPRESSION
#define INERTIA_PARSER_EXPRESSION

/// @file Parser/Expression.h
/// @brief An expression in `inr-isa` is represented by `(operator expr...)`.

#include <inr/TIR/TIRT.h>
#include <isa/Lexer/Lexer.h>

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace isa {

class Value {
public:
    enum ValueID : unsigned {
        String,
        Ident,
        TIRT,
        Expr,
    };

private:
    ValueID id_;

public:
    Value(ValueID id) : id_(id) {}
    virtual ~Value() = default;

    ValueID getID() const {
        return id_;
    }
};

class StringValue : public Value {
    std::string string_;

public:
    StringValue(std::string_view initial) : Value(String), string_(initial) {}

    std::string& access() {
        return string_;
    }

    const std::string& access() const {
        return string_;
    }
};

class IdentValue : public Value {
    std::string_view ident_;

public:
    IdentValue(std::string_view ident) : Value(Ident), ident_(ident) {}

    std::string_view getIdent() const {
        return ident_;
    }
};

class TIRValue : public Value {
    inr::TIRT type_;

public:
    TIRValue(inr::TIRT type) : Value(TIRT), type_(type) {}

    inr::TIRT getType() const {
        return type_;
    }
};

class ExprValue : public Value {
    std::vector<std::unique_ptr<Value>> values_;

public:
    ExprValue() : Value(Expr) {}

    const std::vector<std::unique_ptr<Value>>& getValues() const {
        return values_;
    }

    std::vector<std::unique_ptr<Value>>& getValues() {
        return values_;
    }
};

} // namespace isa

#endif // INERTIA_PARSER_EXPRESSION
