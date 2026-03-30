// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_MIR_MACHINEFUNCTION_H
#define INERTIA_MIR_MACHINEFUNCTION_H

/// @file MIR/MachineFunction.h
/// @brief Contains the machine function class.

#include <inr/ADT/IList.h>
#include <inr/ADT/IVector.h>
#include <inr/ADT/StrView.h>
#include <inr/IR/Value.h>
#include <inr/MIR/MachineBlock.h>
#include <inr/MIR/Register.h>

#include <optional>
#include <unordered_map>
#include <vector>

namespace inr {

/// @brief A machine level function containing machine blocks.
///
/// Mirrors inr::Function but at the MIR level.
/// Tracks frame slots for frame lowering.
class MachineFunction : public ilist_node<MachineFunction> {
    sview name_;
    class MachineModule* parent_;
    ilist<MachineBlock> blocks_;
    std::unordered_map<const Value*, uint32_t> vregMap_;

    /// @brief Frame slot sizes in bytes, indexed by frame index.
    std::vector<uint32_t> frameSlots_;

    /// @brief Next virtual register index.
    uint32_t nextVReg_ = 0;

    explicit MachineFunction(sview name, MachineModule* parent) :
        name_(name), parent_(parent) {}

public:
    MachineModule* getParent() const noexcept {
        return parent_;
    }

    MachineModule* getParent() noexcept {
        return parent_;
    }

    static MachineFunction* create(sview name, MachineModule* parent) {
        return new MachineFunction(name, parent);
    }

    const auto& getVregMap() const noexcept {
        return vregMap_;
    }

    auto& getVregMap() noexcept {
        return vregMap_;
    }

    std::optional<uint32_t> getVregFromValue(const Value* val) {
        auto it = vregMap_.find(val);
        if(it != vregMap_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    /// @brief Allocates a new virtual register and puts the value into the map.
    /// @param val Value to map.
    /// @return The new vreg.
    uint32_t newVregValue(const Value* val) {
        uint32_t vreg = allocateVReg();
        vregMap_[val] = vreg;
        return vreg;
    }

    /// @brief Allocates a new frame slot of the given size.
    /// @param bytes Size of the slot in bytes.
    /// @return Frame index of the new slot.
    uint32_t allocateFrameSlot(uint32_t bytes = 8) {
        frameSlots_.push_back(bytes);
        return frameSlots_.size() - 1;
    }

    /// @brief Allocates a new virtual register.
    /// @return Virtual register index.
    uint32_t allocateVReg() {
        return nextVReg_++;
    }

    /// @brief Returns the total stack size in bytes.
    uint32_t getStackSize() const noexcept {
        uint32_t total = 0;
        for(auto s : frameSlots_) total += s;
        return total;
    }

    /// @brief Returns the frame slot size at the given index.
    uint32_t getFrameSlotSize(uint32_t idx) const {
        return frameSlots_.at(idx);
    }

    MachineBlock* newBlock(sview label) {
        return blocks_.push_back(MachineBlock::create(label, this));
    }

    sview getName() const noexcept {
        return name_;
    }

    const ilist<MachineBlock>& getBlocks() const noexcept {
        return blocks_;
    }
    ilist<MachineBlock>& getBlocks() noexcept {
        return blocks_;
    }

    ~MachineFunction() noexcept {
        for(MachineBlock* b = blocks_.front(); b != nullptr;) {
            MachineBlock* next = b->getNext();
            delete b;
            b = next;
        }
    }
};

} // namespace inr

#endif // INERTIA_MIR_MACHINEFUNCTION_H
