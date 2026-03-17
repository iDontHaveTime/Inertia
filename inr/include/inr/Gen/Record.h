#ifndef INERTIA_GEN_RECORD_H
#define INERTIA_GEN_RECORD_H

/// @file Gen/Record.h
/// @brief Contains the nodes for inr-gen.

#include <inr/ADT/StrView.h>

#include <bit>
#include <memory>
#include <vector>

namespace inr::gen {

class Node {
public:
    enum class NodeType {
        Root,
        Target,
        Operand,
        Instruction,
        InstructionType,
        OperandDesc,
        Type
    };

protected:
    std::vector<std::unique_ptr<Node>> nodes;

    NodeType kind_;

public:
    Node(NodeType kind) noexcept : kind_(kind) {}

    /// @brief Assumes ownership of the pointer.
    std::unique_ptr<Node>& addNode(Node* n) {
        return nodes.emplace_back(n);
    }

    NodeType getKind() const noexcept {
        return kind_;
    }

    const std::vector<std::unique_ptr<Node>>& getNodes() const noexcept {
        return nodes;
    }

    std::vector<std::unique_ptr<Node>>& getNodes() noexcept {
        return nodes;
    }

    virtual ~Node() noexcept = default;
};

class OperandNode : public Node {
    sview name_; ///< Operand name.
public:
    OperandNode(sview name) noexcept : Node(NodeType::Operand), name_(name) {}

    sview getName() const noexcept {
        return name_;
    }
};

class TypeNode : public Node {
public:
    enum class ID { Integer };

private:
    ID id_;
    unsigned width_;

public:
    TypeNode(ID id, unsigned width) noexcept :
        Node(NodeType::Type), id_(id), width_(width) {}

    ID getID() const noexcept {
        return id_;
    }

    unsigned getWidth() const noexcept {
        return width_;
    }
};

class OperandDesc : public Node {
    const OperandNode* operand_;

public:
    OperandDesc(const OperandNode* operand) noexcept :
        Node(NodeType::OperandDesc), operand_(operand) {}

    const OperandNode* getOperand() const noexcept {
        return operand_;
    }

    const TypeNode* getType() const noexcept {
        return nodes.empty() ? nullptr : (const TypeNode*)nodes[0].get();
    }
};

class InstructionTypeNode : public Node {
    sview name_; ///< Operand name.
public:
    InstructionTypeNode(sview name) noexcept :
        Node(NodeType::InstructionType), name_(name) {}

    sview getName() const noexcept {
        return name_;
    }
};

class InstructionNode : public Node {
    const InstructionTypeNode* instType_;

public:
    InstructionNode(const InstructionTypeNode* instType) noexcept :
        Node(NodeType::Instruction), instType_(instType) {}

    const InstructionTypeNode* getInstType() const noexcept {
        return instType_;
    }
};

class TargetNode : public Node {
    sview name_;         ///< Name of the target.
    std::endian endian_; ///< Endian of the target.
    unsigned ptrWidth_;  ///< Pointer width of the target.
public:
    TargetNode(sview name, std::endian endian, unsigned ptrWidth) noexcept :
        Node(NodeType::Target),
        name_(name),
        endian_(endian),
        ptrWidth_(ptrWidth) {}

    sview getName() const noexcept {
        return name_;
    }

    std::endian getEndian() const noexcept {
        return endian_;
    }

    unsigned getPtrWidth() const noexcept {
        return ptrWidth_;
    }
};

} // namespace inr::gen

namespace inr {

raw_stream& operator<<(raw_stream&, std::endian);

}

#endif // INERTIA_GEN_RECORD_H
