// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/ADT/ArrView.h>
#include <inr/Gen/CppEmitter.h>
#include <inr/Gen/Record.h>
#include <inr/Support/Logger.h>
#include <inr/Support/Stream.h>

#include <memory>

namespace inr::gen {

constexpr sview EMITTER_NAME = "inr-gen-emitter";

class CPPDefine {
    sview name_;

public:
    constexpr CPPDefine(sview name) noexcept : name_(name) {}

    constexpr sview getName() const noexcept {
        return name_;
    }
};

class CPPBody {
    raw_stream& os_;
    bool semicolon_;
    bool newline_;

public:
    CPPBody(raw_stream& os, bool semicolon = false,
            bool newline = true) noexcept :
        os_(os), semicolon_(semicolon), newline_(newline) {}

    void close() {
        os_ << '}';
        if(semicolon_) os_ << ';';
        if(newline_) os_ << '\n';
    };

    friend class CPPEmitter;
};

class CPPArg {
    sview type_;
    sview name_;

public:
    constexpr CPPArg(sview type, sview name) noexcept :
        type_(type), name_(name) {}

    constexpr sview getType() const noexcept {
        return type_;
    }

    constexpr sview getName() const noexcept {
        return name_;
    }
};

#define KEYWORD_TO_STRING(x) #x

class CPPEmitter {
    raw_stream& os_;

public:
    CPPEmitter(raw_stream& os) noexcept : os_(os) {}

    inline raw_stream& emit() noexcept {
        return os_;
    }

    inline raw_stream& emit(char c) {
        return emit() << c;
    }

    inline raw_stream& emit(sview addition) {
        return emit() << addition;
    }

    void addLineComment(sview comment) {
        emit("// ") << comment << '\n';
    }

    CPPDefine addIfDef(sview condition) {
        emit("#ifdef ") << condition << '\n';
        return CPPDefine(condition);
    }

    void addEndif() {
        emit("#endif\n");
    }

    void addUndef(CPPDefine define) {
        emit("#undef ") << define.getName() << '\n';
    }

    CPPBody addNamespace(sview name) {
        emit(KEYWORD_TO_STRING(namespace) " inr::") << name << " {\n";

        return CPPBody(os_);
    }

private:
    void printArg(CPPArg& arg, size_t argc) {
        if(argc) emit(", ");
        emit(arg.getType()) << ' ' << arg.getName();
    }

public:
    template<typename... Str>
    void addFunction(sview returnType, sview functionName, Str&&... args) {
        emit(returnType) << ' ' << functionName << '(';

        size_t arg = 0;
        (printArg(args, arg++), ...);

        emit(')');
    }

    template<typename... Str>
    void addFuncDecl(sview returnType, sview functionName, Str&&... args) {
        addFunction(returnType, functionName, std::forward<Str>(args)...);
        emit(";\n");
    }

    template<typename... Str>
    CPPBody addFuncDef(sview returnType, sview functionName, Str&&... args) {
        addFunction(returnType, functionName, std::forward<Str>(args)...);
        emit(" {\n");
        return CPPBody(os_);
    }

    CPPBody addEnumClass(sview name, sview type) {
        emit("enum class ") << name;

        if(!type.empty()) {
            emit(" : ") << type;
        }

        emit(" {\n");

        return CPPBody(os_, true);
    }
};

constexpr sview arrviewTreeNodeObjectFuncS =
    KEYWORD_TO_STRING(arrview<TreeNodeObjectFunc>);

constexpr sview TreeNodeInitializerObjectS =
    KEYWORD_TO_STRING(TreeNodeInitializerObject);

constexpr sview TreeNodeObjectFuncS = KEYWORD_TO_STRING(TreeNodeObjectFunc);

constexpr sview InrInrContextRef = KEYWORD_TO_STRING(const inr::InrContext&);

std::string createStringFromOperandDesc(const InstructionNode* ins) {
    const TypeNode* tn = nullptr;
    bool typesSame = true;

    for(const std::unique_ptr<Node>& descNode : ins->getNodes()) {
        const OperandDescNode* desc = (const OperandDescNode*)descNode.get();
        if(!tn) {
            tn = desc->getType();
        }
        else {
            if(*tn != *desc->getType()) {
                typesSame = false;
                break;
            }
        }
    }
    std::string typeStr;

    for(const std::unique_ptr<Node>& descNode : ins->getNodes()) {
        const OperandDescNode* desc = (const OperandDescNode*)descNode.get();

        if(!typesSame) {
            typeStr += desc->getType()->toString();
        }
        else {
            if(&descNode == ins->getNodes().data()) {
                typeStr += desc->getType()->toString();
            }
        }

        typeStr += desc->getOperand()->getName();
    }

    return typeStr;
}

void emitTarget(CPPEmitter& os, const TargetNode* tn) {
    sview targetName = tn->getName();
    // Target definition: targetName
    os.emit("// Target definition: ") << targetName << '\n';

    // namespace inr::targetName {
    CPPBody ns = os.addNamespace(targetName);

    // From here on this is target's header file.
    CPPDefine th = os.addIfDef("TARGET_HEADER");

    os.emit("constexpr unsigned targetPointerWidth = ")
        << tn->getPtrWidth() << ";\n";
    os.emit("constexpr std::endian targetEndian = std::endian::")
        << tn->getEndian() << ";\n";

    // arrview<TreeNodeObjectFunc> getTargetTree();
    os.addLineComment("Target's tree request function");
    os.addFuncDecl(arrviewTreeNodeObjectFuncS, "getTargetTree");

    // Operands enum
    os.addLineComment("Target's operands");
    CPPBody opsEnum = os.addEnumClass("Operands", "OperandType");

    for(const std::unique_ptr<Node>& node : tn->getNodes()) {
        if(node->getKind() == Node::NodeType::Operand) {
            os.emit('\t') << ((const OperandNode*)node.get())->getName()
                          << ",\n";
        }
    }

    opsEnum.close();

    // Instruction types enum
    os.addLineComment("Target's instruction types");
    CPPBody instTEnum = os.addEnumClass("InstructionTypes", "InstructionType");

    for(const std::unique_ptr<Node>& node : tn->getNodes()) {
        if(node->getKind() == Node::NodeType::InstructionType) {
            os.emit('\t') << ((const InstructionTypeNode*)node.get())->getName()
                          << ",\n";
        }
    }

    instTEnum.close();

    // Opcodes, names made on the fly.
    std::vector<std::string> opcodes;

    os.addLineComment("Target's opcodes");
    CPPBody opcEnum = os.addEnumClass("Opcodes", "OpcodeType");

    for(const std::unique_ptr<Node>& node : tn->getNodes()) {
        if(node->getKind() == Node::NodeType::Instruction) {
            const InstructionNode* ins = (const InstructionNode*)node.get();

            std::string& str =
                opcodes.emplace_back(ins->getInstType()->getName());

            std::string typeStr = createStringFromOperandDesc(ins);

            str += typeStr;

            os.emit('\t') << str << ",\n";
        }
    }

    opcEnum.close();

    os.addUndef(th);
    os.addEndif();
    // Here the target header ended.

    // Target's C++
    CPPDefine ts = os.addIfDef("TARGET_SOURCE");

    size_t idx = 0;
    for(const std::unique_ptr<Node>& node : tn->getNodes()) {
        if(node->getKind() == Node::NodeType::Instruction) {
            const InstructionNode* ins = (const InstructionNode*)node.get();
            std::string& name = opcodes[idx++];

            CPPBody fbody = os.addFuncDef(TreeNodeInitializerObjectS, name,
                                          CPPArg(InrInrContextRef, "ctx"));

            os.emit("\treturn {\"");
            CPPBody initList(os.emit(), true);

            os.emit(name) << "\",\n"
                          << "\t\tInstructionType(InstructionTypes::"
                          << ins->getInstType()->getName()
                          << "),\n\t\t{{\n\t\t\t";

            CPPBody descInit(os.emit(), false, false);

            for(const std::unique_ptr<Node>& descNode : ins->getNodes()) {
                if(&descNode != ins->getNodes().data()) {
                    os.emit(",\n\t\t\t");
                }
                const OperandDescNode* desc =
                    (const OperandDescNode*)descNode.get();
                os.emit("{");
                CPPBody descB(os.emit(), false, false);

                os.emit("OperandType(Operands::")
                    << desc->getOperand()->getName() << "), ctx.";

                switch(desc->getType()->getID()) {
                    case TypeNode::ID::Integer:
                        os.emit("getInt(")
                            << desc->getType()->getWidth() << ")";
                        break;
                }

                descB.close();
            }

            os.emit("\n\t\t");
            descInit.close();
            descInit.close();

            os.emit(",\n\t\t") << "OpcodeType(Opcodes::" << name << ")\n";

            os.emit('\t');
            initList.close();

            fbody.close();
        }
    }

    os.emit(TreeNodeObjectFuncS) << " targetTreeArray[] = {\n";
    CPPBody targetTreeArrayBody(os.emit(), true);

    for(const std::string& name : opcodes) {
        os.emit('\t') << name << ",\n";
    }

    targetTreeArrayBody.close();

    CPPBody gTTFuncBody =
        os.addFuncDef(arrviewTreeNodeObjectFuncS, "getTargetTree");

    os.emit("\treturn {targetTreeArray};\n");

    gTTFuncBody.close();

    os.addUndef(ts);
    os.addEndif();
    // End of target's C++

    // }
    ns.close();

    // End of target: targetName
    os.emit("// End of target: ") << targetName << '\n';
}

#undef KEYWORD_TO_STRING

void emitter::emit(raw_stream& os, const std::unique_ptr<Node>& tree) {
    CPPEmitter cppemitter(os);
    // Add an extra new line for padding
    cppemitter.addLineComment("File generated by inr-gen.\n");

    if(tree->getKind() != Node::NodeType::Root) {
        log::send(errs(), log::Level::ERROR, EMITTER_NAME,
                  "expected root node");
        return;
    }
    for(const std::unique_ptr<Node>& node : tree->getNodes()) {
        if(node->getKind() == Node::NodeType::Target) {
            emitTarget(cppemitter, (const TargetNode*)node.get());
        }
        else {
            log::send(errs(), log::Level::ERROR, EMITTER_NAME,
                      "unexpected node found while emitting");
        }
    }
}

} // namespace inr::gen