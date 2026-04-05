// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TIRINSTRUCTION_H
#define INERTIA_TIR_TIRINSTRUCTION_H

/// @file TIR/TIRInstruction.h
/// @brief Contains the Target IR instruction class.

namespace inr {

/// @brief Possible TIR instructions.
enum class TIRInstID {
    // Start
    TIR_INST_START,

    // End
    TIR_INST_END
};

/// @brief Represents a TIR instruction.
/// @note This is the base class, and should not be used.
class TIRInstruction {
    TIRInstID id_;

protected:
    /// @brief Constructs a new TIR instruction.
    TIRInstruction(TIRInstID id) noexcept : id_(id) {}

public:
    /// @brief Returns the instruction type.
    TIRInstID getInstID() const noexcept {
        return id_;
    }

    virtual ~TIRInstruction() noexcept = default;
};

} // namespace inr

#endif // INERTIA_TIR_TIRINSTRUCTION_H
