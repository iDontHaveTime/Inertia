#include <inr/ISel/TargetTree.h>
#include <inr/Support/Stream.h>

namespace inr {

const LeafNode* Walker::walk(const TargetTree* root, const Type* keyType,
                             uint32_t keyInstT, uint16_t keyOP) {
    const TargetTree* current = root;

    bool done = false;
    while(!done) {
        switch(current->getNodeType()) {
            case TargetTree::NodeType::Type:
                current = ((const TypeTree*)current)->getNode(keyType);
                break;
            case TargetTree::NodeType::InstructionType:
                current = ((const InstructionTree*)current)->getNode(keyInstT);
                break;
            case TargetTree::NodeType::Operand:
                current = ((const OperandTree*)current)->getNode(keyOP);
                break;
            case TargetTree::NodeType::Leaf:
                done = true;
                break;
        }
        if(!current) done = true;
    }

    return (const LeafNode*)current;
}

TargetTree* TreeNodeBuilder::buildTree() {
    if(!exists_) {
        TypeTree* root = (TypeTree*)storage_.emplace_back(new TypeTree()).get();

        for(const TreeNodeObjectFunc& func : nodes_) {
            TreeNodeInitializerObject obj = func(ctx_);
            InstructionTree* inst =
                (InstructionTree*)root->getNode(obj.getType());
            if(!inst)
                inst =
                    (InstructionTree*)root
                        ->newNode(
                            obj.getType(),
                            storage_.emplace_back(new InstructionTree()).get())
                        .ptr;

            OperandTree* op = (OperandTree*)inst->getNode(obj.getInstType());
            if(!op)
                op = (OperandTree*)inst
                         ->newNode(
                             obj.getInstType(),
                             storage_.emplace_back(new OperandTree()).get())
                         .ptr;

            LeafNode* leaf = (LeafNode*)op->getNode(obj.getOperands());
            if(!leaf)
                op->newNode(
                    obj.getOperands(),
                    storage_
                        .emplace_back(new LeafNode(obj.getOp(), obj.getName()))
                        .get());
        }

        exists_ = true;
    }
    return storage_.data()->get();
}

raw_stream& printNode(raw_stream& os, const TargetTree& node,
                      unsigned& indent) {
    switch(node.getNodeType()) {
        case TargetTree::NodeType::Type:
            for(const auto& p : ((const TypeTree&)node).getNodes()) {
                os.indent(indent) << *p.first << ":\n";
                if(p.second) {
                    indent++;
                    printNode(os, *p.second, indent);
                    indent--;
                }
            }
            break;
        case TargetTree::NodeType::InstructionType:
            for(const auto& p : ((const InstructionTree&)node).getNodes()) {
                os.indent(indent) << p.first << ":\n";
                if(p.second) {
                    indent++;
                    printNode(os, *p.second, indent);
                    indent--;
                }
            }
            break;
        case TargetTree::NodeType::Operand:
            for(const auto& p : ((const OperandTree&)node).getNodes()) {
                os.indent(indent) << p.first << ":\n";
                if(p.second) {
                    indent++;
                    printNode(os, *p.second, indent);
                    indent--;
                }
            }
            break;
        case TargetTree::NodeType::Leaf:
            return os.indent(indent)
                   << ((const LeafNode&)node).getName() << '\n';
    }

    return os;
}

raw_stream& operator<<(raw_stream& os, const TargetTree& node) {
    unsigned indent = 0;
    return printNode(os, node, indent);
}

namespace x86 {
arrview<TreeNodeObjectFunc> getTargetTree();
}

arrview<TreeNodeObjectFunc> getTargetTreeInit(Triple T) {
    switch(T.getArch()) {
        case Triple::Arch::x86_64:
            return x86::getTargetTree();
        case Triple::Arch::Unknown:
            [[fallthrough]];
        default:
            return {};
    }
}

} // namespace inr