// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/ADT/ArrView.h>
#include <inr/Gen/CppEmitter.h>
#include <inr/Gen/Record.h>
#include <inr/Support/Logger.h>
#include <inr/Support/Stream.h>

// REFERENCE
//
// class Target<string name, int ptrSize, endian targetEndian> {
//     string Name = name;
//     string Namespace = name;
//     int PointerSize = ptrSize;
//     endian Endian = targetEndian;
// }
//
// class Register<string name, int size> {
//     string Name = name;
//     int Size = size;
//     list<Register> SubRegs = [];
// }
//
// class RegisterClass<int size, list<Register> regs> {
//     int Size = size;
//     list<Register> Regs = regs;
// }
//
// class CallingConv<list<Register> args, list<Register> ret, list<Register>
// callerSaved,
//                   list<Register> calleeSaved, Register stackPtr, Register
//                   framePtr>{
//     list<Register> Args = args;
//     list<Register> Ret = ret;
//     list<Register> CallerSaved = callerSaved;
//     list<Register> CalleeSaved = calleeSaved;
//     Register StackPtr = stackPtr;
//     Register FramePtr = framePtr;
// }

namespace inr::gen {

struct InrGenInfo {
    bool Success = false;
    using gen_string = sview;
    using gen_int = unsigned;
    using gen_endian = std::endian;
    using gen_list = const std::vector<const Init*>*;
    using gen_def = const Record*;
};

struct TargetInfo : public InrGenInfo {
    gen_string Name;
    gen_string Namespace;
    gen_int PointerSize;
    gen_endian Endian;

    TargetInfo() noexcept = default;

    TargetInfo(const Record* target) {
        const Init* NameInit = target->getField("Name");
        const Init* NamespaceInit = target->getField("Namespace");
        const Init* PointerSizeInit = target->getField("PointerSize");
        const Init* EndianInit = target->getField("Endian");

        if(!NameInit || !NamespaceInit || !PointerSizeInit || !EndianInit)
            return;

        if(!NameInit->matches(RecordType::Kind::String)) return;
        if(NamespaceInit->getKind() != NameInit->getKind()) return;
        if(!PointerSizeInit->matches(RecordType::Kind::Integer)) return;
        if(!EndianInit->matches(RecordType::Kind::Endian)) return;

        Name = StringInit::get(NameInit);
        Namespace = StringInit::get(NamespaceInit);
        PointerSize = IntegerInit::get(PointerSizeInit);
        Endian = EndianInit::get(EndianInit);

        Success = true;
    }
};

template<typename... Args>
static inline void vError(Args&&... args) {
    log::sendargs(errs(), log::Level::ERROR, "inr-gen-backend",
                  std::forward<Args>(args)...);
}
} // namespace inr::gen

namespace inr::gen {

// REGISTER BACKEND

struct RegisterClassInfo : public InrGenInfo {
    gen_int Size;
    gen_list Regs;

    RegisterClassInfo(const Record* gpr) {
        const Init* SizeInit = gpr->getField("Size");
        const Init* RegsInit = gpr->getField("Regs");

        if(!SizeInit || !RegsInit) return;

        if(!SizeInit->matches(RecordType::Kind::Integer)) return;
        if(!RegsInit->matches(RecordType::Kind::List)) return;

        Size = IntegerInit::get(SizeInit);
        Regs = &ListInit::get(RegsInit);

        Success = true;
    }
};

struct RegisterInfo : public InrGenInfo {
    gen_string Name;
    gen_int Size;
    gen_list SubRegs;

    RegisterInfo(const Record* reg) {
        const Init* NameInit = reg->getField("Name");
        const Init* SizeInit = reg->getField("Size");
        const Init* SubRegsInit = reg->getField("SubRegs");

        if(!NameInit || !SizeInit || !SubRegsInit) return;

        if(!NameInit->matches(RecordType::Kind::String)) return;
        if(!SizeInit->matches(RecordType::Kind::Integer)) return;
        if(!SubRegsInit->matches(RecordType::Kind::List)) return;

        Name = StringInit::get(NameInit);
        Size = IntegerInit::get(SizeInit);
        SubRegs = &ListInit::get(SubRegsInit);

        Success = true;
    }
};

static inline TargetInfo getTargetInfo(const RecordStorage& result) {
    std::vector<const Record*> targetDef = result.getDefsDerivedFrom("Target");
    if(targetDef.empty() || targetDef.size() > 1) {
        vError("there must be only 1 target def");
        return {};
    }

    return {targetDef.front()};
}

static inline bool generateRegisterList(raw_stream& os,
                                        InrGenInfo::gen_list reglist) {
    bool err = false;
    size_t subIdx = 0;
    for(const Init* subInit : *reglist) {
        if(subIdx++) {
            os << ", ";
        }
        if(!subInit->matches(RecordType::Kind::Def)) {
            err = true;
            break;
        }
        const Record* subReg = ((const DefInit*)subInit)->getValue();
        os << subReg->getName();
    }
    os << '\n';
    return err;
}

bool RegisterBackend::emit(const RecordStorage& result) {
    TargetInfo target = getTargetInfo(result);
    if(!target.Success) return true;

    std::vector<const Record*> registers =
        result.getDefsDerivedFrom("Register");
    std::vector<const Record*> registerClasses =
        result.getDefsDerivedFrom("RegisterClass");

    addComment("Generated by inr-gen, backend: Register");

    bool err = false;

    constexpr sview REGISTER_HEADER_MACRO = "_REGISTER_HEADER";

    // REGISTER HEADER
    addIfDef(target.Namespace, REGISTER_HEADER_MACRO);
    addUndef(target.Namespace, REGISTER_HEADER_MACRO);

    // NAMESPACE
    addNamespace(target.Namespace);
    openBody();

    size_t currentIdx = 0;
    for(const Record* reg : registers) {
        RegisterInfo info(reg);
        if(!info.Success) {
            err = true;
            break;
        }

        writeln("constexpr Register ", reg->getName(),
                " = Register::createPhysical(", currentIdx++, ");");

        if(!info.SubRegs->empty()) {
            write("constexpr Register ", reg->getName(), "_SubRegs[] = ");
            openBody();

            generateRegisterList(os_, info.SubRegs);

            closeBody(true);
        }
        if(err) break;
    }

    for(const Record* regClass : registerClasses) {
        RegisterClassInfo info(regClass);
        if(!info.Success) {
            err = true;
            break;
        }

        write("constexpr Register ", regClass->getName(), "[] = ");
        openBody();

        generateRegisterList(os_, info.Regs);

        closeBody(true);
    }

    closeBody();
    // END OF NAMESPACE

    addEndIf();
    // END OF REGISTER HEADER

    return err;
}

// CALLING CONV BACKEND

struct CallingConvInfo : public InrGenInfo {
    gen_list Args;
    gen_list Ret;
    gen_list CallerSaved;
    gen_list CalleeSaved;
    gen_def StackPtr;
    gen_def FramePtr;

    CallingConvInfo(const Record* conv) {
        const Init* ArgsInit = conv->getField("Args");
        const Init* RetInit = conv->getField("Ret");
        const Init* CallerSavedInit = conv->getField("CallerSaved");
        const Init* CalleeSavedInit = conv->getField("CalleeSaved");
        const Init* StackPtrInit = conv->getField("StackPtr");
        const Init* FramePtrInit = conv->getField("FramePtr");

        if(!ArgsInit || !RetInit || !CallerSavedInit || !CalleeSavedInit ||
           !StackPtrInit || !FramePtrInit)
            return;

        if(!ArgsInit->matches(RecordType::Kind::List)) return;
        if(!RetInit->matches(RecordType::Kind::List)) return;
        if(!CallerSavedInit->matches(RecordType::Kind::List)) return;
        if(!CalleeSavedInit->matches(RecordType::Kind::List)) return;
        if(!StackPtrInit->matches(RecordType::Kind::Def)) return;
        if(!FramePtrInit->matches(RecordType::Kind::Def)) return;

        Args = &ListInit::get(ArgsInit);
        Ret = &ListInit::get(RetInit);
        CallerSaved = &ListInit::get(CallerSavedInit);
        CalleeSaved = &ListInit::get(CalleeSavedInit);

        StackPtr = DefInit::get(StackPtrInit);
        FramePtr = DefInit::get(FramePtrInit);

        Success = true;
    }
};

bool CallingConvBackend::emit(const RecordStorage& result) {
    TargetInfo target = getTargetInfo(result);
    if(!target.Success) return true;

    std::vector<const Record*> convs = result.getDefsDerivedFrom("CallingConv");

    addComment("Generated by inr-gen, backend: Calling Convention");

    bool err = false;

    constexpr sview CALLING_CONV_HEADER_MACRO = "_CALLING_CONV_HEADER";

    // CALLING CONV HEADER
    addIfDef(target.Namespace, CALLING_CONV_HEADER_MACRO);
    addUndef(target.Namespace, CALLING_CONV_HEADER_MACRO);

    // NAMESPACE
    addNamespace(target.Namespace);
    openBody();

    for(const Record* conv : convs) {
        CallingConvInfo info(conv);
        if(!info.Success) {
            err = true;
            break;
        }

        sview convName = conv->getName();

        write("constexpr Register ", convName, "_Args[] = ");
        openBody();
        generateRegisterList(os_, info.Args);
        closeBody(true);

        write("constexpr Register ", convName, "_Ret[] = ");
        openBody();
        generateRegisterList(os_, info.Ret);
        closeBody(true);

        write("constexpr Register ", convName, "_CallerSaved[] = ");
        openBody();
        generateRegisterList(os_, info.CallerSaved);
        closeBody(true);

        write("constexpr Register ", convName, "_CalleeSaved[] = ");
        openBody();
        generateRegisterList(os_, info.CalleeSaved);
        closeBody(true);

        writeln("constexpr Register ", convName,
                "_StackPtr = ", info.StackPtr->getName(), ';');
        writeln("constexpr Register ", convName,
                "_FramePtr = ", info.FramePtr->getName(), ';');
    }

    closeBody();
    // END OF NAMESPACE

    addEndIf();
    // END OF CALLING CONV HEADER

    return err;
}

} // namespace inr::gen