// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_SEMA_SEMA_H
#define INRCC_SEMA_SEMA_H

/// @file Sema/Sema.h
/// @brief Provides the semantic analysis class.

#include <inrcc/ADT/PointerMap.h>
#include <inrcc/AST/DeclAST.h>
#include <inrcc/Lexer/Lexer.h>

namespace inrcc {

class Sema {
    PtrMap<const IdentInfo*, Decl*> symbTable_;
    unsigned scope_;

public:
    Decl* lookupIdent(const IdentInfo* info) const noexcept {
        return symbTable_.find(info);
    }

    void setIdent(const IdentInfo* key, Decl* val) noexcept {
        symbTable_.insert(key, val);
    }

    unsigned getScope() const noexcept {
        return scope_;
    }

    void incScope() noexcept {
        scope_++;
    }

    void decScope() noexcept {
        scope_--;
    }
};

} // namespace inrcc

#endif // INRCC_SEMA_SEMA_H
