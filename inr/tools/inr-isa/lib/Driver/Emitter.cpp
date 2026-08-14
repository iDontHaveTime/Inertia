// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/ADT/HMap.h>
#include <inr/Support/Assert.h>
#include <inr/Support/Stream.h>
#include <inr/TIR/TIRT.h>
#include <isa/Driver/Emitter.h>
#include <isa/Parser/Expression.h>

#include <string_view>
#include <vector>

namespace isa {

class RegisterEmitter : public Emitter {
    bool emit(inr::stream&,
              const std::vector<std::unique_ptr<Value>>&) const override;
};

class FeatureEmitter : public Emitter {
    bool emit(inr::stream&,
              const std::vector<std::unique_ptr<Value>>&) const override;
};

constexpr static std::string_view registerEmitterHelpText_{
    R"(This backend emits a C++ file that defines the target registers, register classes, and register families.
Usually to start off you would define the namespace, so for example:
(namespace inr 
    (namespace TargetName // for example x86
        // Registers would go here.
    )
)
Then to define a register it follows the (register REGISTER_NAME REGISTER_STRING) format.
So to define a register like RAX it would be (register RAX "rax").
Then defining register families, even if the target has registers with only one size.
The register family expression follows the (register_family FAMILY_NAME REGISTERS...) format.
Example for a family would be the A family on x86 like (register_family A_FAMILY AL AH AX EAX RAX).
Then finally are the register classes, which follow the (register_class CLASS_NAME TIR_TYPE REGISTERS...) format.
Defining a register class would look like this (register_class GP64 tir<b64> RAX RBX RCX etc..).s
)"};

constexpr static std::string_view featureEmitterHelpText_{
    R"(This backend emits a C++ file that defines target features.
To make a target feature, we would first define a namespace like this:
(namspace inr
    (namespace TargetName // for example x86
        // Target features here.
    )
)
A target feature defintion follows this:
(feature name default)
So a feature like avx2 would be defined as:
(feature avx2 false)
Which means a feature named avx2 exists on this target, and by default its turned off.    
)"};

static const RegisterEmitter registerEmitter_;
static const FeatureEmitter featureEmitter_;

static EmitterBackend available_emitters[] = {
    {"register", &registerEmitter_, registerEmitterHelpText_},
    {"feature", &featureEmitter_, featureEmitterHelpText_}};

const EmitterBackend* Emitter::getByName(std::string_view view) {
    for(const EmitterBackend& p : available_emitters) {
        if(view == p.name) return &p;
    }
    return nullptr;
}

inr::arrview<EmitterBackend> Emitter::getAvailableEmitters() {
    return {available_emitters,
            sizeof(available_emitters) / sizeof(*available_emitters)};
}

template<typename... Args>
static inline void printError(Args&&... args) {
    ((inr::err() << "emitter: ").changeColor(inr::col::RED, true) << "error: ")
        .resetColor();
    ((inr::err() << args), ...);
    inr::err() << '\n';
}

struct EmitterCtxGeneric {
    std::vector<std::string_view> namespaces;
};

struct RegisterEmitterCtx : EmitterCtxGeneric {
    inr::HMap<std::string_view, unsigned> registerIDMap;
    std::vector<std::pair<std::string_view, std::string_view>> registers;
    std::vector<std::pair<std::string_view, std::vector<unsigned>>>
        register_families;
    std::vector<std::pair<std::pair<std::string_view, inr::TIRT>,
                          std::vector<unsigned>>>
        register_classes;
};

template<typename Ctx>
static inline bool parseNamespace(const ExprValue* expr, Ctx& ctx,
                                  bool (*parseFunc)(const ExprValue*, Ctx&)) {
    if(expr->getValues().size() > 1) {
        if(expr->getValues()[1]->getID() == Value::Ident) {
            std::string_view namespace_name =
                ((const IdentValue*)expr->getValues()[1].get())->getIdent();
            ctx.namespaces.emplace_back(namespace_name);
            for(auto it = expr->getValues().begin() + 2;
                it != expr->getValues().end(); ++it) {
                if(it->get()->getID() == Value::Expr) {
                    if(parseFunc((const ExprValue*)it->get(), ctx)) return true;
                }
                else {
                    printError("couldn't process namespace '", namespace_name,
                               "' because the values after were "
                               "not expressions");
                    return true;
                }
            }
        }
        else {
            printError("couldn't emit namespace, no identifier was found");
            return true;
        }
    }
    return false;
}

static inline bool registerEmitterExpr(const ExprValue* expr,
                                       RegisterEmitterCtx& ctx) {
    if(!expr->getValues().empty()) {
        const Value* op = expr->getValues()[0].get();
        if(op->getID() == Value::Ident) {
            const IdentValue* ident = (const IdentValue*)op;
            if(ident->getIdent() == "namespace") {
                return parseNamespace(expr, ctx, registerEmitterExpr);
            }
            else if(ident->getIdent() == "register") {
                if(expr->getValues().size() != 3) {
                    printError(
                        "register does not match signature of (register "
                        "IDENT "
                        "STRING)");
                    return true;
                }

                if(expr->getValues()[1]->getID() != Value::Ident) {
                    printError(
                        "register operator second value should be an "
                        "identifier");
                    return true;
                }

                if(expr->getValues()[2]->getID() != Value::String) {
                    printError(
                        "register operator third value should be a string");
                    return true;
                }

                const IdentValue* regName =
                    (const IdentValue*)expr->getValues()[1].get();
                const StringValue* regStr =
                    (const StringValue*)expr->getValues()[2].get();

                unsigned regNum = ctx.registers.size();

                auto [_, e] =
                    ctx.registerIDMap.try_emplace(regName->getIdent(), regNum);
                if(!e) {
                    printError("couldn't emplace register '",
                               regName->getIdent(), "' into register map");
                    return true;
                }

                ctx.registers.emplace_back(regName->getIdent(),
                                           regStr->access());
            }
            else if(ident->getIdent() == "register_family") {
                if(expr->getValues().size() < 2) {
                    printError(
                        "register_family does not match signature of "
                        "(register_family IDENT...)");
                    return true;
                }
                for(const auto& v : expr->getValues()) {
                    if(v->getID() != Value::Ident) {
                        printError(
                            "everything should be an ident in "
                            "register_family");
                        return true;
                    }
                }
                const IdentValue* familyName =
                    (const IdentValue*)expr->getValues()[1].get();
                ctx.register_families.emplace_back(familyName->getIdent(),
                                                   std::vector<unsigned>{});

                for(auto it = expr->getValues().begin() + 2;
                    it != expr->getValues().end(); ++it) {
                    const IdentValue* regName = (const IdentValue*)it->get();
                    auto val = ctx.registerIDMap.find(regName->getIdent());
                    if(!val) {
                        printError("register '", regName->getIdent(),
                                   "' not found");
                        continue;
                    }
                    ctx.register_families.back().second.emplace_back(*val);
                }
                std::sort(ctx.register_families.back().second.begin(),
                          ctx.register_families.back().second.end());
            }
            else if(ident->getIdent() == "register_class") {
                if(expr->getValues().size() < 3) {
                    printError(
                        "register_class does not match signature of "
                        "(register_class IDENT TIR IDENT...)");
                    return true;
                }
                if(expr->getValues()[1]->getID() != Value::Ident) {
                    printError(
                        "value after the operator should be an identifier "
                        "in "
                        "register_class");
                    return true;
                }
                if(expr->getValues()[2]->getID() != Value::TIRT) {
                    printError(
                        "value after the identifier should be a TIR type "
                        "in "
                        "register_class");
                    return true;
                }

                for(auto it = expr->getValues().begin() + 3;
                    it != expr->getValues().end(); ++it) {
                    if((*it)->getID() != Value::Ident) {
                        printError(
                            "everything should be an ident in "
                            "register_class "
                            "after the first two after operator values");
                        return true;
                    }
                }

                const IdentValue* className =
                    (const IdentValue*)expr->getValues()[1].get();
                const TIRValue* tirType =
                    (const TIRValue*)expr->getValues()[2].get();
                ctx.register_classes.emplace_back(
                    std::pair<std::string_view, inr::TIRT>{
                        className->getIdent(), tirType->getType()},
                    std::vector<unsigned>{});

                for(auto it = expr->getValues().begin() + 3;
                    it != expr->getValues().end(); ++it) {
                    const IdentValue* regName = (const IdentValue*)it->get();
                    auto val = ctx.registerIDMap.find(regName->getIdent());
                    if(!val) {
                        printError("register '", regName->getIdent(),
                                   "' not found");
                        continue;
                    }
                    ctx.register_classes.back().second.emplace_back(*val);
                }
                std::sort(ctx.register_classes.back().second.begin(),
                          ctx.register_classes.back().second.end());
            }
            else {
                printError("unknown operator '", ident->getIdent(), '\'');
            }
        }
    }
    return false;
}

template<typename Ctx>
static inline bool parseAndEmit(inr::stream& os,
                                const std::vector<std::unique_ptr<Value>>& vals,
                                bool (*parseFunc)(const ExprValue*, Ctx&),
                                void (*emitFunc)(inr::stream&, const Ctx&)) {
    for(const auto& exprVal : vals) {
        Ctx ctx;
        inr_assert(exprVal->getID() == Value::Expr,
                   "parseAndEmit(): value is not an expression");
        if(parseFunc((const ExprValue*)exprVal.get(), ctx)) return true;

        emitFunc(os, ctx);
    }
    return false;
}

class CPPNamespace {
    inr::stream& os_;

public:
    CPPNamespace(inr::stream& os, const EmitterCtxGeneric& ctx) : os_(os) {
        os << "namespace ";
        for(unsigned i = 0; i < ctx.namespaces.size(); i++) {
            if(i) os << "::";
            os << ctx.namespaces[i];
        }
        os << " {\n";
    }

    ~CPPNamespace() {
        os_ << "}\n";
    }
};

static inline void registerEmitterEmit(inr::stream& os,
                                       const RegisterEmitterCtx& ctx) {
    CPPNamespace n(os, ctx);

    os << "#ifdef TARGET_REGISTER_DEFINITIONS\n";
    os << "constexpr Register target_registers[] = {\n";
    for(unsigned i = 0; i < ctx.registers.size(); i++) {
        os.indent(4) << "{\"" << ctx.registers[i].second << "\", " << i
                     << "},\n";
    }
    os << "};\n";

    for(const auto& v : ctx.register_families) {
        os << "constexpr uint32_t " << v.first << "_regs[] = {\n";
        for(unsigned j : v.second) {
            os.indent(4) << j << ",\n";
        }
        os << "};\n";
    }

    os << "constexpr RegisterFamily target_register_families[] = {\n";
    for(unsigned i = 0; i < ctx.register_families.size(); i++) {
        const auto& v = ctx.register_families[i];
        os.indent(4) << '{' << i << ", " << v.first << "_regs},\n";
    }
    os << "};\n";

    for(const auto& v : ctx.register_classes) {
        os << "constexpr uint32_t " << v.first.first << "_regs[] = {\n";
        for(unsigned j : v.second) {
            os.indent(4) << j << ",\n";
        }
        os << "};\n";
    }

    os << "constexpr RegisterClass target_register_classes[] = {\n";
    for(unsigned i = 0; i < ctx.register_classes.size(); i++) {
        const auto& v = ctx.register_classes[i];
        os.indent(4) << '{' << i << ", " << v.first.first << "_regs, ";
        switch(v.first.second.getKind()) {
            case inr::TIRT::Bit:
                os << "inr::TIRT::createBit(" << v.first.second.getWidth()
                   << ')';
                break;
            case inr::TIRT::Other:
                os << "inr::TIRT::createOther()";
                break;
        }
        os << "},\n";
    }
    os << "};\n";
    os << "#endif\n";

    for(unsigned i = 0; i < ctx.registers.size(); i++) {
        os << "constexpr uint32_t " << ctx.registers[i].first << " = " << i
           << ";\n";
    }

    for(unsigned i = 0; i < ctx.register_families.size(); i++) {
        os << "constexpr uint32_t " << ctx.register_families[i].first << " = "
           << i << ";\n";
    }

    for(unsigned i = 0; i < ctx.register_classes.size(); i++) {
        os << "constexpr uint32_t " << ctx.register_classes[i].first.first
           << " = " << i << ";\n";
    }
}

bool RegisterEmitter::emit(
    inr::stream& os, const std::vector<std::unique_ptr<Value>>& vals) const {
    os << "#include <inr/Target/Register.h>\n";
    return parseAndEmit(os, vals, registerEmitterExpr, registerEmitterEmit);
}

struct FeatureEmitterCtx : EmitterCtxGeneric {
    std::vector<std::pair<std::string_view, bool>> features;
};

static inline bool featureEmitterExpr(const ExprValue* expr,
                                      FeatureEmitterCtx& ctx) {
    if(!expr->getValues().empty()) {
        const Value* op = expr->getValues()[0].get();
        if(op->getID() == Value::Ident) {
            const IdentValue* ident = (const IdentValue*)op;
            if(ident->getIdent() == "namespace") {
                return parseNamespace(expr, ctx, featureEmitterExpr);
            }
            else if(ident->getIdent() == "feature") {
                if(expr->getValues().size() != 3) {
                    printError(
                        "features must follow the (feature IDENT true|false) "
                        "format");
                    return true;
                }
                if(expr->getValues()[1]->getID() != Value::Ident) {
                    printError("feature's first value must be an identifier");
                    return true;
                }
                if(expr->getValues()[2]->getID() != Value::Ident ||
                   (((const IdentValue*)expr->getValues()[2].get())
                            ->getIdent() != "true" &&
                    ((const IdentValue*)expr->getValues()[2].get())
                            ->getIdent() != "false")) {
                    printError(
                        "feature's second value should be a true or false");
                    return true;
                }
                std::string_view featName =
                    ((const IdentValue*)expr->getValues()[1].get())->getIdent();
                bool featDefault =
                    ((const IdentValue*)expr->getValues()[2].get())
                        ->getIdent() == "true";
                ctx.features.emplace_back(featName, featDefault);
            }
            else {
                printError("unknown operator '", ident->getIdent(), '\'');
            }
        }
    }
    return false;
}

static inline void featureEmitterEmit(inr::stream& os,
                                      const FeatureEmitterCtx& ctx) {
    CPPNamespace n(os, ctx);
    os << "struct CustomTargetFeatures : public TargetFeatures {\n";

    for(const auto& p : ctx.features) {
        os.indent(4) << "bool " << p.first << " = " << p.second << ";\n";
    }

    os << "};\n";
}

bool FeatureEmitter::emit(
    inr::stream& os, const std::vector<std::unique_ptr<Value>>& vals) const {
    os << "#include <inr/Target/TargetDesc.h>\n";
    return parseAndEmit(os, vals, featureEmitterExpr, featureEmitterEmit);
}

} // namespace isa
