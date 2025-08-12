#include "Inertia/Target/TargetCodegen.hpp"
#include "Inertia/Target/TargetOutput.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

namespace Inertia{

struct TargetCodegenCTX{
    std::ofstream hpp;
    std::ofstream cpp;
    const TargetOutput& inp;
    std::string structName;
    std::string regcName;

    TargetCodegenCTX() = delete;

    TargetCodegenCTX(const TargetOutput& out, const std::filesystem::path& hpppath, const std::filesystem::path& cpppath) :
        hpp(hpppath), cpp(cpppath), inp(out){
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
    ctx.cpp<<"namespace InertiaTarget"<<'{'<<std::endl;

    ctx.hpp<<"namespace Inertia"<<ctx.inp.target<<'{'<<std::endl;
    ctx.cpp<<"namespace Inertia"<<ctx.inp.target<<'{'<<std::endl;
}

inline void FinalizeFileGen(TargetCodegenCTX& ctx){
    ctx.hpp<<'}'<<std::endl;
    ctx.hpp<<'}'<<std::endl;

    ctx.cpp<<'}'<<std::endl;
    ctx.cpp<<'}'<<std::endl;
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

    // extensions
    for(const ExtensionEntry& ext : ctx.inp.extensions){
        ctx.hpp<<'\t'<<"bool "<<ext.name<<" = false;"<<std::endl;
    }

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

constexpr const char* func_type = "TargetInstructionResult";

std::ofstream& WriteFunction(const InstructionEntry& ins, std::ofstream& stream){
    stream<<func_type<<' '<<ins.name<<'(';

    stream<<"std::ostream& osout";
    if(!ins.ops.empty()){
        stream<<", ";
    }

    for(const InstructionOperand& op : ins.ops){
        if(op.type == TargetParserType::REGISTER){
            stream<<"const Register_"<<op.name<<'*'<<' '<<op.name;
        }
        else if(op.type == TargetParserType::REGCLASS){
            stream<<"const RegisterBase*"<<' '<<op.extra_name;
        }
        else{
            continue;
        }
        if(&op != &ins.ops.back()){
            stream<<", ";
        }
    }
    // args

    stream<<')';
    return stream;
}

std::ofstream& WriteFormat(const InstructionEntry& ins, std::ofstream& stream){
    stream<<mac_str(constexpr)<<" std::string_view "<<ins.name<<"_fmt"<<" = \""<<ins.fmt.fmt<<'"';
    return stream;
}

bool DeclareFunctions(TargetCodegenCTX& ctx){
    if(ctx.inp.instructions.empty()) return false;

    for(const InstructionEntry& ins : ctx.inp.instructions){
        // header
        WriteFunction(ins, ctx.hpp)<<';'<<std::endl;

        // cpp
        WriteFunction(ins, ctx.cpp)<<'{'<<std::endl;
        
        ctx.cpp<<'\t';
        WriteFormat(ins, ctx.cpp)<<';'<<std::endl;

        ctx.cpp<<'\t'<<"std::format_to(std::ostreambuf_iterator<char>(osout), "<<ins.name<<"_fmt";

        if(!ins.fmt.formatees.empty()){
            ctx.cpp<<", ";
            for(const InstructionFormatee& fmtee : ins.fmt.formatees){
                ctx.cpp<<fmtee.name;
                switch(fmtee.field){
                    case InstructionFormatee::FormatField::NONE:
                        break;
                    case InstructionFormatee::FormatField::NAME:
                        ctx.cpp<<"->name";
                        break;
                }
                if(&fmtee != &ins.fmt.formatees.back()){
                    ctx.cpp<<", ";
                }
            }
        }

        ctx.cpp<<')'<<';'<<std::endl;

        ctx.cpp<<'\t'<<"return {"<<std::endl;

        ctx.cpp<<"\t\t.result = "<<ins.result<<','<<std::endl;
        ctx.cpp<<"\t\t.clobbers = {";
        for(size_t i = 0; i < ins.clobberC; i++){
            ctx.cpp<<ins.clobbers[i];
            if(i + 1 != ins.clobberC){
                ctx.cpp<<", ";
            }
        }
        ctx.cpp<<'}'<<','<<std::endl;

        ctx.cpp<<"\t\t.clobberSize = "<<std::to_string(ins.clobberC)<<std::endl;

        ctx.cpp<<'\t'<<'}'<<';'<<std::endl;;

        ctx.cpp<<'}'<<std::endl;
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

    std::filesystem::path cpppath = fullpath;
    cpppath += ".cpp";

    TargetCodegenCTX ctx(input, headerpath, cpppath);

    if(!ctx.hpp){
        return true;
    }

    GenerateHeaderGuardsTop(ctx);

    WriteLineComment(ctx.hpp, "This file is auto generated by Inertia");
    
    // writes cppinc
    WriteInclude(ctx.cpp, headerpath, true);

    if(!ctx.inp.instructions.empty()){
        ctx.cpp<<"#include <format>"<<std::endl;
    }
    WriteLineComment(ctx.cpp, "This file is auto generated by Inertia");

    WriteIncludes(ctx);
    // writes internal includes
    ctx.hpp<<'\n';
    
    WriteInclude(ctx.hpp, "Inertia/Target/TargetBase.hpp");
    
    StartFileGen(ctx);

    WriteRegisterClasses(ctx);
    WriteRegisters(ctx);
    WriteBaseClass(ctx);

    // body

    DeclareFunctions(ctx);

    FinalizeFileGen(ctx);

    ctx.hpp<<'\n';

    GenerateHeaderGuardsBottom(ctx);

    return false;
}

}