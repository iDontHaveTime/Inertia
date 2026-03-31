// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_IR_RUNNER_H
#define INERTIA_IR_RUNNER_H

/// @file IR/Runner.h
/// @brief Provides a class for running modules.

#include <inr/IR/Constant.h>
#include <inr/IR/Context.h>
#include <inr/IR/Module.h>
#include <inr/Target/Triple.h>

namespace inr {

/// @brief Runs the main() function in the module.
///
/// Main must have a signature of `i32 @main(i32 %argc, ptr %argv)`.
class ModuleRunner {
public:
    /// @brief Runs the module and returns the result from main.
    static const ConstantInt* execute(const Module* mod, const InrContext& ctx,
                                      Triple target, int argc, char** argv);
};

} // namespace inr

#endif // INERTIA_IR_RUNNER_H
