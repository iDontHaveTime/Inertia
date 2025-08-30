#include "Inertia/Target/TargetCodegen.hpp"
#include "Inertia/Target/TargetOutput.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <string>
#include <string_view>

namespace Inertia{

struct TargetCodegenCTX{
    std::ofstream hpp;
    const TargetOutput& inp;
    std::string structName;
    std::string regcName;

    TargetCodegenCTX() = delete;

    TargetCodegenCTX(const TargetOutput& out, const std::filesystem::path& hpppath) :
        hpp(hpppath), inp(out){
        // names
        structName = "TargetBase";
        structName += out.target;
        regcName = "RegisterClass";
        regcName += out.target;
    };
};

inline void WriteInclude(std::ofstream& stream, const std::filesystem::path& inc, bool global = false){
    if(!global)
        stream<<"#include \""<<inc.generic_string()<<'"'<<std::endl;
    else
        stream<<"#include \"../"<<inc.generic_string()<<'"'<<std::endl;
}

inline void GenerateHeaderGuardsTop(TargetCodegenCTX& ctx){
    ctx.hpp<<"#ifndef "<<"INERTIA_GENERATED_"<<ctx.inp.target<<std::endl;
    ctx.hpp<<"#define "<<"INERTIA_GENERATED_"<<ctx.inp.target<<std::endl;
}

inline void GenerateHeaderGuardsBottom(TargetCodegenCTX& ctx){
    ctx.hpp<<"#endif"<<std::endl;
}

inline void StartFileGen(TargetCodegenCTX& ctx){
    ctx.hpp<<"namespace InertiaTarget"<<'{'<<std::endl;

    ctx.hpp<<"namespace Inertia"<<ctx.inp.target<<'{'<<std::endl;
}

inline void FinalizeFileGen(TargetCodegenCTX& ctx){
    ctx.hpp<<'}'<<std::endl;
    ctx.hpp<<'}'<<std::endl;
}

inline void WriteTabs(std::ofstream& stream, int n){
    while(n--){
        stream<<'\t';
    }
}

bool WriteData(TargetCodegenCTX& ctx, const DataEntry& de, int tabs){
    bool cons = false;
    for(const Data& dt : de.data){
        WriteTabs(ctx.hpp, tabs);
        ctx.hpp<<"uint64_t "<<dt.name;

        ctx.hpp<<" : "<<std::to_string(dt.width);

        ctx.hpp<<';'<<std::endl;
        if(dt.had_default) cons = true;
    }
    return cons;
}

void InitData(TargetCodegenCTX& ctx, const DataEntry& de, int tabs, bool custom = false, uintmax_t with = 0){
    for(const Data& dt : de.data){
        if(!dt.had_default && !custom) continue;
        WriteTabs(ctx.hpp, tabs);
        ctx.hpp<<dt.name<<" = ";
        if(custom){
            ctx.hpp<<std::to_string(with);
        }
        else{
            ctx.hpp<<std::to_string(dt.def_init);
        }
        ctx.hpp<<';'<<std::endl;
    }
}

#define mac_str(x) #x

void WriteBaseClass(TargetCodegenCTX& ctx){
    ctx.hpp<<"struct "<<ctx.structName<<" : public TargetBase"<<'{'<<std::endl;

    // buffer
    ctx.hpp<<"\tchar* buff = nullptr;"<<std::endl;

    // regs
    for(const RegisterEntry& regen : ctx.inp.registers){
        ctx.hpp<<'\t'<<"Register_"<<regen.name<<'*'<<' '<<regen.name<<';'<<std::endl;
    }

    // constructor
    ctx.hpp<<'\t'<<ctx.structName<<'('<<')';

    ctx.hpp<<" : "<<"TargetBase"<<'(';

    ctx.hpp<<"Inertia::Endian::";
    switch(ctx.inp.endian){
        case Endian::IN_UNKNOWN_ENDIAN:
            ctx.hpp<<mac_str(IN_UNKNOWN_ENDIAN);
            break;
        case Endian::IN_LITTLE_ENDIAN:
            ctx.hpp<<mac_str(IN_LITTLE_ENDIAN);
            break;
        case Endian::IN_BIG_ENDIAN:
            ctx.hpp<<mac_str(IN_BIG_ENDIAN);
            break;
    }

    ctx.hpp<<')';

    ctx.hpp<<'{'<<std::endl;

    // extensions
    for(const ExtensionEntry& ext : ctx.inp.extensions){
        ctx.hpp<<"\t\textensions[\""<<ext.name<<"\"]"<<" = false;"<<std::endl;
    }

    ctx.hpp<<"\t\tinit();"<<std::endl;

    ctx.hpp<<'\t'<<'}'<<std::endl;
    // end constructor

    ctx.hpp<<'\t'<<"void init() override"<<'{'<<std::endl;

    // allocate
    ctx.hpp<<"\t\tbuff = new char[(";
    for(const RegisterEntry& regen : ctx.inp.registers){
        ctx.hpp<<"sizeof("<<"Register_"<<regen.name<<')';
        if(&regen != &ctx.inp.registers.back()){
            ctx.hpp<<'+';
        }
    }
    ctx.hpp<<")];"<<std::endl;

    ctx.hpp<<"\t\tchar* buff_ptr = buff;"<<std::endl;

    for(const RegisterEntry& regen : ctx.inp.registers){
        ctx.hpp<<'\t'<<'\t'<<regen.name<<" = new(buff_ptr) Register_"<<regen.name<<"();"<<std::endl;
        ctx.hpp<<'\t'<<'\t'<<"buff_ptr += sizeof("<<"Register_"<<regen.name<<')'<<';'<<std::endl;
        ctx.hpp<<'\t'<<'\t'<<"reg_database"<<'['<<'"'<<regen.name<<'"'<<']'<<" = "<<regen.name<<';'<<std::endl;
    }

    // set
    for(const RegisterEntry& regen : ctx.inp.registers){
        if(!regen.parent.empty()){
            ctx.hpp<<'\t'<<'\t'<<regen.name<<"->set_parent"<<'('<<regen.parent<<')'<<';'<<std::endl;
        }
    }

    ctx.hpp<<'\t'<<'}'<<std::endl;


    // destructor

    ctx.hpp<<'\t'<<'~'<<ctx.structName<<"() noexcept override"<<'{'<<std::endl;

    for(auto it = ctx.inp.registers.rbegin(); it != ctx.inp.registers.rend(); it++){
        ctx.hpp<<'\t'<<'\t'<<it->name<<"->~Register_"<<it->name<<"();"<<std::endl;
    }

    ctx.hpp<<"\t\tdelete[] buff;"<<std::endl;

    ctx.hpp<<'\t'<<'}'<<std::endl;

    // end destructor

    ctx.hpp<<"};"<<std::endl;
}

void WriteRegisterClasses(TargetCodegenCTX& ctx){
    if(ctx.inp.regclasses.empty()) return;
    ctx.hpp<<"enum class "<<ctx.regcName<<'{'<<std::endl;

    for(const std::string_view& sw : ctx.inp.regclasses){
        ctx.hpp<<'\t'<<sw<<','<<std::endl;
    }

    ctx.hpp<<"};"<<std::endl;
}

inline void CastTo(std::ofstream& stream, const std::string_view& type){
    stream<<'('<<type<<')';
}

inline void WriteLineComment(std::ofstream& stream, const std::string_view& comment){
    stream<<"// "<<comment<<std::endl;
}

bool WriteIncludes(TargetCodegenCTX& ctx){
    for(const CPPInclude& cppinc : ctx.inp.cppinc){
        ctx.hpp<<"#include ";
        if(cppinc.type == CPPInclude::CPPIncludeType::Arrows){
            ctx.hpp<<'<';
        }
        else{
            ctx.hpp<<'"';
        }

        ctx.hpp<<cppinc.name;

        if(cppinc.type == CPPInclude::CPPIncludeType::Arrows){
            ctx.hpp<<'>';
        }
        else{
            ctx.hpp<<'"';
        }

        ctx.hpp<<std::endl;
    }

    return false;
}

bool WriteRegisters(TargetCodegenCTX& ctx){
    if(ctx.inp.registers.empty()){
        return true;
    }

    for(const RegisterEntry& regen : ctx.inp.registers){
        bool cons = false;
        ctx.hpp<<"struct "<<"Register_"<<regen.name<<" : public RegisterBase"<<'{'<<std::endl;

        for(size_t i : regen.dataIndeces){
            if(WriteData(ctx, ctx.inp.datas[i], 1)){
                cons = true;
            }
        }
        if(regen.inits.size() > 0) cons = true;

        ctx.hpp<<'\t'<<"Register_"<<regen.name<<'('<<')'<<" : "<<"RegisterBase"<<'(';

        ctx.hpp<<'"'<<regen.name<<'"'<<", ";

        CastTo(ctx.hpp, "int");
        ctx.hpp<<ctx.regcName<<"::"<<regen.classname<<", ";

        char buff[128];

        snprintf(buff, 128, "%d", regen.width);

        ctx.hpp<<buff<<')'<<'{';

        if(!cons){
            ctx.hpp<<'}'<<';'<<std::endl;
        }
        else{
            ctx.hpp<<std::endl;

            for(const std::string& sw : regen.init){
                ctx.hpp<<'\t'<<'\t'<<sw<<std::endl;
            }

            for(size_t i : regen.dataIndeces){
                InitData(ctx, ctx.inp.datas[i], 2);
            }

            size_t field = 0;
            for(const DataInitEntry& din : regen.inits){
                if(!din.init){
                    field++;
                    continue;
                }
                const auto& dt = ctx.inp.datas[din.di].data[field];

                ctx.hpp<<'\t'<<'\t'<<dt.name<<" = "<<std::to_string(din.val)<<';'<<std::endl;
                field++;
            }

            ctx.hpp<<'\t'<<'}'<<std::endl;
        }

        ctx.hpp<<"};"<<std::endl;
    }

    return false;
}

void TypeBasedOnWidth(uint32_t w, std::ostream& os){
    if(w <= 8){
        os<<"uint8_t";
    }
    else if(w <= 16){
        os<<"uint16_t";
    }
    else if(w <= 32){
        os<<"uint32_t";
    }
    else if(w <= 64){
        os<<"uint64_t";
    }
    else{
        os<<"__uint128_t";
    }
}

bool DeclareClasses(TargetCodegenCTX& ctx){
    if(ctx.inp.instructions.empty()) return false;

    ctx.hpp<<"enum class InstrID"<<ctx.inp.target<<" : uint32_t"<<'{'<<std::endl;

    for(const InstructionEntry& ins : ctx.inp.instructions){
        ctx.hpp<<'\t'<<ins.name;
        if(&ins != &ctx.inp.instructions.back()){
            ctx.hpp<<',';
        }
        ctx.hpp<<std::endl;
    }

    ctx.hpp<<'}'<<';'<<std::endl;

    for(const InstructionEntry& ins : ctx.inp.instructions){
        ctx.hpp<<"struct Instr"<<ins.name<<" : public TargetInstruction"<<'{'<<std::endl;

        // vars
        for(const InstructionOperand& op : ins.ops){
            switch(op.type){
                case TargetParserType::REGCLASS:
                    ctx.hpp<<'\t'<<"RegisterBase* "<<op.extra_name<<';'<<std::endl;
                    break;
                case TargetParserType::REGISTER:
                    ctx.hpp<<'\t'<<"Register_"<<op.name<<"* "<<op.name<<';'<<std::endl;
                    break;
                case TargetParserType::STRING:
                    ctx.hpp<<'\t'<<"std::string_view "<<op.name<<';'<<std::endl;
                    break;
                case TargetParserType::IMMEDIATE:
                    ctx.hpp<<'\t';
                    TypeBasedOnWidth(op.extras, ctx.hpp);
                    ctx.hpp<<' '<<op.name<<';'<<std::endl;
                    break;
                default:
                    return true;
            }
        }

        // constructor
        ctx.hpp<<"\tInstr"<<ins.name<<'(';

        for(const InstructionOperand& op : ins.ops){
            switch(op.type){
                case TargetParserType::REGCLASS:
                    ctx.hpp<<"RegisterBase* _"<<op.extra_name<<'_';
                    break;
                case TargetParserType::REGISTER:
                    ctx.hpp<<"Register_"<<op.name<<"* _"<<op.name<<'_';
                    break;
                case TargetParserType::STRING:
                    ctx.hpp<<"const std::string_view& _"<<op.name<<'_';
                    break;
                case TargetParserType::IMMEDIATE:
                    TypeBasedOnWidth(op.extras, ctx.hpp);
                    ctx.hpp<<" _"<<op.name<<'_';
                    break;
                default:
                    return true;
            }
            if(&op != &ins.ops.back()){
                ctx.hpp<<", ";
            }
        }

        ctx.hpp<<')'<<" : ";

        ctx.hpp<<"TargetInstruction(";

        CastTo(ctx.hpp, "uint32_t");
        ctx.hpp<<"InstrID"<<ctx.inp.target<<"::"<<ins.name<<", ";

        ctx.hpp<<'{';

        if(ins.result.empty()){
            ctx.hpp<<".result = nullptr, ";
        }
        else{
            ctx.hpp<<".result = _"<<ins.result<<"_, ";
        }

        ctx.hpp<<".clobbers = {";

        for(size_t i = 0; i < ins.clobberC; i++){
            ctx.hpp<<'_'<<ins.clobbers[i]<<'_';

            if(i + 1 < ins.clobberC){
                ctx.hpp<<", ";
            }
        }

        ctx.hpp<<"}, .clobberSize = "<<std::to_string(ins.clobberC);

        ctx.hpp<<'}';

        ctx.hpp<<')';

        for(const InstructionOperand& op : ins.ops){
            ctx.hpp<<", ";
            switch(op.type){
                case TargetParserType::REGCLASS:
                    ctx.hpp<<op.extra_name<<"(_"<<op.extra_name<<"_)";
                    break;
                case TargetParserType::IMMEDIATE:
                    [[fallthrough]];
                case TargetParserType::REGISTER:
                    [[fallthrough]];
                case TargetParserType::STRING:
                    ctx.hpp<<op.name<<"(_"<<op.name<<"_)";
                    break;
                default:
                    return true;
            }
        }

        ctx.hpp<<"{};"<<std::endl;

        // end of constructor

        // emit
        ctx.hpp<<"\tvoid emit(std::ostream& os) const override"<<'{'<<std::endl;

        ctx.hpp<<"\t\tstd::format_to(std::ostreambuf_iterator<char>(os), \""<<ins.fmt.fmt<<'"';

        if(!ins.fmt.formatees.empty()){
            ctx.hpp<<", ";
            for(const InstructionFormatee& fmtee : ins.fmt.formatees){
                ctx.hpp<<fmtee.name;

                switch(fmtee.field){
                    case InstructionFormatee::FormatField::NONE:
                        break;
                    case InstructionFormatee::FormatField::NAME:
                        ctx.hpp<<"->name";
                        break;
                }

                if(&fmtee != &ins.fmt.formatees.back()){
                    ctx.hpp<<", ";
                }
            }
        }

        ctx.hpp<<')'<<';'<<std::endl;;

        ctx.hpp<<'\t'<<'}'<<std::endl;

        // end
        ctx.hpp<<'}'<<';'<<std::endl;
    }

    return false;
}

bool TargetCodegen::output(){
    if(!input.file){
        return true;
    }

    if(input.target.empty()) return true;
    if(input.file.get_path().empty()) return true;

    std::filesystem::path fullpath = input.file.get_path();

    std::filesystem::path headerpath = fullpath;
    headerpath += ".hpp";

    TargetCodegenCTX ctx(input, headerpath);

    if(!ctx.hpp){
        return true;
    }

    GenerateHeaderGuardsTop(ctx);

    WriteLineComment(ctx.hpp, "This file is auto generated by Inertia");

    if(WriteIncludes(ctx)){
        return true;
    }
    // writes internal includes
    ctx.hpp<<'\n';

    if(!ctx.inp.instructions.empty()){
        ctx.hpp<<"#include <format>"<<std::endl;
    }

    WriteInclude(ctx.hpp, "Inertia/Target/TargetBase.hpp");

    StartFileGen(ctx);

    WriteRegisterClasses(ctx);
    if(WriteRegisters(ctx)){
        return true;
    }
    WriteBaseClass(ctx);

    // body

    if(DeclareClasses(ctx)){
        return true;
    }

    FinalizeFileGen(ctx);

    ctx.hpp<<'\n';

    GenerateHeaderGuardsBottom(ctx);

    return false;
}

}
