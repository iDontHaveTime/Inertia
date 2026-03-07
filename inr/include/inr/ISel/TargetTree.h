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
    enum class NodeType : uint8_t { Type, InstructionType, Operand, Leaf };

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
///
/// A new node type can be created easily via the `using` keyword.
/// Below this class you can find these two lines:
/// `using TypeTree = TargetTreeNode<const Type*, TargetTree::NodeType::Type>`,
/// `using OperandTree = TargetTreeNode<OperandType,
/// TargetTree::NodeType::Operand>`. The first line being the type tree node
/// creates a new node type that can hold other nodes based on the type, so for
/// example you could hold RR (Reg-Reg) node under the i32 type.
/// The second line creates a tree node that is splitting the nodes based on
/// their operands. To create your own node you just put the key as the first
/// template argument, and the second template argument is what type of node it
/// is. Although this class shouldn't be used outside of this header, since this
/// is mainly to reduce code repetition rather than provide a generic tree
/// class.
template<typename T, TargetTree::NodeType nodeType>
class TargetTreeNode : public TargetTree {
protected:
    std::unordered_map<T, TargetTree*> nodes_;

public:
    TargetTreeNode() noexcept(
        std::is_nothrow_default_constructible_v<decltype(nodes_)>) :
        TargetTree(nodeType) {}

    const std::unordered_map<T, TargetTree*>& getNodes() const noexcept {
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

/// @brief A node that holds other nodes based on the type.
using TypeTree = TargetTreeNode<const Type*, TargetTree::NodeType::Type>;
/// @brief A node that holds other nodes based on the operands.
using OperandTree = TargetTreeNode<OperandType, TargetTree::NodeType::Operand>;
/// @brief A node that holds other nodes based on an integer.
using InstructionTree =
    TargetTreeNode<uint32_t, TargetTree::NodeType::InstructionType>;

class LeafNode : public TargetTree {
    OpcodeType op_;

public:
    LeafNode(OpcodeType op) noexcept : TargetTree(NodeType::Leaf), op_(op) {}

    OpcodeType getOp() const noexcept {
        return op_;
    }
};

/// @brief Walks the tree based on the conditions given.
class Walker {
public:
    /// @brief Walks from the root to the node with the given keys.
    /// @param root The node to start from.
    /// @param keyType The type of the requested node.
    /// @param keyInstT The instruction type of the requested node.
    /// @param keyOP The operands of the requested node.
    /// @return The node if found, nullptr if not found.
    static const LeafNode* walk(const TargetTree* root, const Type* keyType,
                                InstructionType keyInstT, OperandType keyOP);
};

/// @brief Used to make a flat array of instructions that are then converted to
/// a tree.
class TreeNodeInitializerObject {
    const Type* type_; ///< Type of the instruction, as in integer, float, etc..
    InstructionType
        instType_; ///<  Type of the instruction, as in add, sub, mul, etc..
    OperandType
        ops_; ///< Operands of the instruction, as in Reg-Reg, Reg-Mem, etc..
    OpcodeType op_; ///< Opcode of the instruction, used by emitters.

public:
    /// @brief Basic initialization of a node object.
    /// @param type Type for the node.
    /// @param instType Instruction type for the node.
    /// @param ops Operands for the node.
    /// @param op Opcode for the leaf node.
    constexpr TreeNodeInitializerObject(const Type* type,
                                        InstructionType instType,
                                        OperandType ops, OpcodeType op) noexcept
        :
        type_(type), instType_(instType), ops_(ops), op_(op) {}

    constexpr const Type* getType() const noexcept {
        return type_;
    }

    constexpr InstructionType getInstType() const noexcept {
        return instType_;
    }

    constexpr OperandType getOperands() const noexcept {
        return ops_;
    }

    constexpr OpcodeType getOp() const noexcept {
        return op_;
    }
};

using TreeNodeObjectFunc = TreeNodeInitializerObject (*)(class InrContext&);

/// @brief Builds a tree from a list of tree initializer objects.
///
/// This class builds the tree only once, thus buildTree() can be called however
/// many times.
class TreeNodeBuilder {
    InrContext& ctx_;
    std::vector<TreeNodeObjectFunc> nodes_;
    std::vector<std::unique_ptr<TargetTree>> storage_;
    bool exists_;

public:
    TreeNodeBuilder(const TreeNodeBuilder&) = delete;
    TreeNodeBuilder& operator=(const TreeNodeBuilder&) = delete;

    TreeNodeBuilder(InrContext& ctx,
                    std::initializer_list<TreeNodeObjectFunc> nodes) :
        ctx_(ctx), nodes_(nodes), storage_(), exists_(false) {}

    TreeNodeBuilder(InrContext& ctx, arrview<TreeNodeObjectFunc> nodes) :
        ctx_(ctx), nodes_(nodes.vec()), storage_(), exists_(false) {}

    TargetTree* buildTree();
};

raw_stream& operator<<(raw_stream&, const TargetTree&);

} // namespace inr

#endif // INERTIA_ISEL_TARGETTREE_H
