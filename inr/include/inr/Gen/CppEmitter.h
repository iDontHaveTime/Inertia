// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_GEN_CPPEMITTER_H
#define INERTIA_GEN_CPPEMITTER_H

/// @file Gen/CppEmitter.h
/// @brief Emits C++ from the inr-gen files.

#include <inr/Gen/Record.h>
#include <inr/Support/Stream.h>

#include <memory>

namespace inr::gen {

class emitter {
public:
    /// @brief Emits C++ from the tree provided.
    /// @param os The stream to output it to.
    /// @param tree The tree from the parser.
    static void emit(raw_stream& os, const std::unique_ptr<Node>& tree);
};

} // namespace inr::gen

#endif // INERTIA_GEN_CPPEMITTER_H
