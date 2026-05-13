// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_AST_DECLAST_H
#define INRCC_AST_DECLAST_H

/// @file DeclAST.h
/// @brief Provides the declaration ASTs.

#include <inr/ADT/StrView.h>

namespace inrcc {

class Decl {
public:
    enum Kind { TypeDecl };
    enum Storage : uint8_t {
        Default = 0,
        Extern = 0x1,
        Static = 0x2,
        AutoStorage = 0x4,
        Register = 0x8,
        Typedef = 0x10
    };

private:
    Kind kind_;

public:
    Decl(Kind kind) noexcept : kind_(kind) {}

    Kind getKind() const noexcept {
        return kind_;
    }
};

class DeclNamed : public Decl {
    inr::sview name_;

public:
    DeclNamed(Kind kind, inr::sview name) noexcept : Decl(kind), name_(name) {}

    inr::sview getName() const noexcept {
        return name_;
    }
};

} // namespace inrcc

#endif // INRCC_AST_DECLAST_H
