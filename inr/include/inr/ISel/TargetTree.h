#ifndef INERTIA_ISEL_TARGETTREE_H
#define INERTIA_ISEL_TARGETTREE_H

/// @file ISel/TargetTree.h
/// @brief Contains the target tree base class.

#include <inr/ADT/ArrView.h>
#include <inr/ADT/StrView.h>
#include <inr/IR/Type.h>

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <unordered_map>

#include "inr/Target/Triple.h"

namespace inr {

/// @brief Result returned by the target tree related methods.
enum class TreeErrc : uint8_t {
    SUCCESS, ///< Returned if the operation was successful.
    EXISTS,  ///< Returned if a node already exists.
    ERROR    ///< Returned on error, it is preferred to use specific result.
};

/// @brief Base class for target trees.
class TargetTree {
public:
    enum class NodeType : uint8_t { InstructionType, Operand, Leaf };

protected:
    NodeType nodeType_;
    TargetTree(NodeType nodeType) noexcept : nodeType_(nodeType) {}

public:
    NodeType getNodeType() const noexcept {
        return nodeType_;
    }

    virtual ~TargetTree() noexcept = default;
};

struct TreeResult {
    TreeErrc res;
    TargetTree* ptr;
};

/// @brief Node that holds other nodes based on T.
template<typename T, TargetTree::NodeType nodeType,
         typename Hash = std::hash<T>>
class TargetTreeNode : public TargetTree {
protected:
    std::unordered_map<T, TargetTree*, Hash> nodes_;

public:
    TargetTreeNode() noexcept(
        std::is_nothrow_default_constructible_v<decltype(nodes_)>) :
        TargetTree(nodeType) {}

    const std::unordered_map<T, TargetTree*, Hash>& getNodes() const noexcept {
        return nodes_;
    }

    /// @brief Requests a node with the provided T.
    /// @param t The T to find.
    /// @return The node if found, nullptr if not.
    const TargetTree* getNode(T t) const {
        auto it = nodes_.find(t);
        if(it != nodes_.end()) return it->second;
        return nullptr;
    }

    /// @brief Requests a node with the provided T.
    /// @param t The T to find.
    /// @return The node if found, nullptr if not.
    TargetTree* getNode(T t) {
        auto it = nodes_.find(t);
        if(it != nodes_.end()) return it->second;
        return nullptr;
    }

    /// @brief Inserts a new node to the tree.
    /// @param condition Key to find the next node.
    /// @param result The node given if the key is found.
    /// @return `TreeResult::SUCCESS` if inserted, `TreeResult::EXISTS` if it
    /// already exists.
    TreeResult newNode(T condition, TargetTree* result) {
        auto [it, inserted] = nodes_.try_emplace(condition, result);
        return {.res = inserted ? TreeErrc::SUCCESS : TreeErrc::EXISTS,
                .ptr = it->second};
    }
};

/// @brief This should be used when making an enum for operands.
using OperandType = uint16_t;
/// @brief This should be used when making an enum for instruction types.
using InstructionType = uint32_t;
/// @brief This should be used when making an enum for opcodes.
using OpcodeType = uint32_t;

/// @brief A single operand descriptor.
class OperandDesc {
    OperandType id_;
    const Type* type_;

public:
    constexpr OperandDesc(OperandType id, const Type* type) noexcept :
        id_(id), type_(type) {}

    constexpr OperandType getID() const noexcept {
        return id_;
    }

    constexpr const Type* getType() const noexcept {
        return type_;
    }

    constexpr bool operator==(const OperandDesc& o) const noexcept {
        return id_ == o.id_ && type_ == o.type_;
    }
};

/// @brief Signature of all operands for an instruction.
/// Used as the key in OperandTree.
class OperandSignature {
    std::vector<OperandDesc> operands_;

public:
    constexpr OperandSignature(std::vector<OperandDesc> operands) :
        operands_(std::move(operands)) {}

    constexpr const std::vector<OperandDesc>& getOperands() const noexcept {
        return operands_;
    }

    constexpr bool operator==(const OperandSignature& o) const noexcept {
        return operands_ == o.operands_;
    }
};

struct OperandSignatureHash {
    constexpr size_t operator()(const OperandSignature& sig) const noexcept {
        size_t hash = 0;
        for(const OperandDesc& op : sig.getOperands())
            hash ^= std::hash<uint16_t>{}(op.getID()) + 0x9e3779b9 +
                    (hash << 6) + (hash >> 2);
        return hash;
    }
};

/// @brief A node that holds other nodes based on the operands.
using OperandTree =
    TargetTreeNode<OperandSignature, TargetTree::NodeType::Operand,
                   OperandSignatureHash>;
/// @brief A node that holds other nodes based on an integer.
using InstructionTree =
    TargetTreeNode<uint32_t, TargetTree::NodeType::InstructionType>;

class LeafNode : public TargetTree {
    OpcodeType op_;
    sview name_;

public:
    LeafNode(OpcodeType op, sview name) noexcept :
        TargetTree(NodeType::Leaf), op_(op), name_(name) {}

    OpcodeType getOp() const noexcept {
        return op_;
    }

    sview getName() const noexcept {
        return name_;
    }
};

/// @brief Walks the tree based on the conditions given.
class Walker {
public:
    /// @brief Walks from the root to the leaf with the given keys.
    /// @param root The node to start from.
    /// @param keyInstT The instruction type of the requested node.
    /// @param keySig The operand signature of the requested node.
    /// @return The leaf if found, nullptr if not found.
    static const LeafNode* walk(const TargetTree* root,
                                InstructionType keyInstT,
                                const OperandSignature& keySig);
};

/// @brief Used to make a flat array of instructions that are then converted to
/// a tree.
class TreeNodeInitializerObject {
    sview name_; ///< The name of the instruction, as in add64rr, mov32rr, etc..
    InstructionType
        instType_; ///<  Type of the instruction, as in add, sub, mul, etc..
    OperandSignature
        ops_; ///< Operands of the instruction, as in Reg-Reg, Reg-Mem, etc..
    OpcodeType op_; ///< Opcode of the instruction, used by emitters.

public:
    /// @brief Basic initialization of a node object.
    /// @param name The name of the instruction.
    /// @param instType Instruction type for the node.
    /// @param ops Operands for the node.
    /// @param op Opcode for the leaf node.
    constexpr TreeNodeInitializerObject(sview name, InstructionType instType,
                                        OperandSignature ops,
                                        OpcodeType op) noexcept :
        name_(name), instType_(instType), ops_(std::move(ops)), op_(op) {}

    constexpr sview getName() const noexcept {
        return name_;
    }
    constexpr InstructionType getInstType() const noexcept {
        return instType_;
    }
    constexpr const OperandSignature& getOperands() const noexcept {
        return ops_;
    }
    constexpr OpcodeType getOp() const noexcept {
        return op_;
    }
};

using TreeNodeObjectFunc =
    TreeNodeInitializerObject (*)(const class InrContext&);

/// @brief Builds a tree from a list of tree initializer objects.
///
/// This class builds the tree only once, thus buildTree() can be called however
/// many times.
class TreeNodeBuilder {
    const InrContext& ctx_;
    std::vector<TreeNodeObjectFunc> nodes_;
    std::vector<std::unique_ptr<TargetTree>> storage_;
    bool exists_;

public:
    TreeNodeBuilder(const TreeNodeBuilder&) = delete;
    TreeNodeBuilder& operator=(const TreeNodeBuilder&) = delete;

    TreeNodeBuilder(const InrContext& ctx,
                    std::initializer_list<TreeNodeObjectFunc> nodes) :
        ctx_(ctx), nodes_(nodes), storage_(), exists_(false) {}

    TreeNodeBuilder(const InrContext& ctx, arrview<TreeNodeObjectFunc> nodes) :
        ctx_(ctx), nodes_(nodes.vec()), storage_(), exists_(false) {}

    TargetTree* buildTree();
};

arrview<TreeNodeObjectFunc> getTargetTreeInit(Triple);

raw_stream& operator<<(raw_stream&, const TargetTree&);

} // namespace inr

#endif // INERTIA_ISEL_TARGETTREE_H
