#ifndef INERTIA_INR_PASSMANAGER_H
#define INERTIA_INR_PASSMANAGER_H

/// @file PassManager.h
/// @brief Contains the pass manager class.

#include <inr/IR/Context.h>
#include <inr/IR/Module.h>
#include <inr/ISel/TargetTree.h>
#include <inr/MIR/MachineModule.h>
#include <inr/Target/Triple.h>

#include <memory>
#include <vector>

namespace inr {

/// @brief Abstract base class for IR passes.
///
/// IR passes operate on the IR module before instruction selection.
/// Examples: DCE, constant folding, inlining.
class IRPass {
public:
    virtual void run(const InrContext& ctx, Module& mod) = 0;
    virtual ~IRPass() = default;
};

template<typename T>
concept IRPassC = std::derived_from<T, IRPass>;

/// @brief Abstract base class for ISel.
///
/// The ISel pass creates a new machine module from an IR module.
class ISelPass {
public:
    virtual void run(const InrContext& ctx, Module& mod, MachineModule& mmod,
                     TargetTree* tt) = 0;
    virtual ~ISelPass() = default;
};

template<typename T>
concept ISelPassC = std::derived_from<T, ISelPass>;

/// @brief Abstract base class for MIR passes.
///
/// MIR passes operate on the machine module after instruction selection.
/// Examples: register allocation, frame lowering, peephole optimization.
class MIRPass {
public:
    virtual void run(MachineModule& mod, TargetTree* tt) = 0;
    virtual ~MIRPass() = default;
};

template<typename T>
concept MIRPassC = std::derived_from<T, MIRPass>;

/// @brief Drives the full compilation pipeline for a single module.
///
/// The pipeline is split into three explicit stages:
/// - IR passes: optimize SSA IR before lowering
/// - ISel: lower IR to MIR via the target pattern tree
/// - MIR passes: optimize and finalize MIR before emission
///
/// Each stage can be run independently, allowing inspection of
/// intermediate results at any point. run() is a convenience method
/// that runs all three stages in order.
///
/// @see DESIGN.md for architecture details.
class PassManager {
    Triple triple_;
    const InrContext& ctx_;
    Module& mod_;

    TreeNodeBuilder treeBuilder_;
    std::unique_ptr<MachineModule> mmod_;

    std::vector<std::unique_ptr<IRPass>> irPasses_;
    std::unique_ptr<ISelPass> iselPass_;
    std::vector<std::unique_ptr<MIRPass>> mirPasses_;

public:
    PassManager(Triple triple, const InrContext& ctx, Module& mod) :
        triple_(triple),
        ctx_(ctx),
        mod_(mod),
        treeBuilder_(ctx_, getTargetTreeInit(triple_)) {}

    /// @brief Adds an IR pass to the pipeline.
    template<IRPassC PassT, typename... Args>
    void addIRPass(Args&&... args) {
        irPasses_.emplace_back(
            std::make_unique<PassT>(std::forward<Args>(args)...));
    }

    /// @brief Sets the ISel pass.
    /// @note Only one ISel pass is allowed.
    template<ISelPassC PassT, typename... Args>
    void setISel(Args&&... args) {
        iselPass_ = std::make_unique<PassT>(std::forward<Args>(args)...);
    }

    void setISel(std::unique_ptr<ISelPass> pass) {
        iselPass_ = std::move(pass);
    }

    /// @brief Adds a MIR pass to the pipeline.
    template<MIRPassC PassT, typename... Args>
    void addMIRPass(Args&&... args) {
        mirPasses_.emplace_back(
            std::make_unique<PassT>(std::forward<Args>(args)...));
    }

    /// @brief Runs all IR passes on the module.
    void runIRPasses() {
        for(auto& pass : irPasses_) pass->run(ctx_, mod_);
    }

    /// @brief Runs instruction selection, producing a MachineModule.
    void runISel() {
        mmod_ = std::make_unique<MachineModule>(mod_.getName());
        if(iselPass_) iselPass_->run(ctx_, mod_, *mmod_, getTargetTree());
    }

    /// @brief Runs all MIR passes on the machine module.
    void runMIRPasses() {
        for(auto& pass : mirPasses_) pass->run(*mmod_, getTargetTree());
    }

    /// @brief Convenience method that runs all three stages in order.
    void run() {
        runIRPasses();
        runISel();
        runMIRPasses();
    }

    /// @brief Returns the machine module produced by ISel.
    /// @return nullptr if ISel has not been run yet.
    MachineModule* getMachineModule() const noexcept {
        return mmod_.get();
    }

    /// @brief Transfers ownership of the machine module to the caller.
    std::unique_ptr<MachineModule> takeMachineModule() {
        return std::move(mmod_);
    }

    /// @brief Returns the target tree.
    TargetTree* getTargetTree() noexcept {
        return treeBuilder_.buildTree();
    }

    /// @brief Returns the target triple.
    Triple getTriple() const noexcept {
        return triple_;
    }
};

} // namespace inr

#endif // INERTIA_INR_PASSMANAGER_H
