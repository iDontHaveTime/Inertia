#include "Inertia/Target/TargetCodegen.hpp"
#include "Inertia/Target/TargetOutput.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

namespace Inertia{

struct TargetCodegenCTX{
    std::ofstream cpp;
    std::ofstream hpp;
    const TargetOutput& inp;
    std::string structName;
    std::string regcName;

    TargetCodegenCTX() = delete;

    TargetCodegenCTX(const TargetOutput& out, const std::filesystem::path& cpppath, const std::filesystem::path& hpppath) :
        cpp(cpppath), hpp(hpppath), inp(out){
        // names
        structName = "TargetBase" + out.target;
        regcName = "RegisterClass" + out.target;
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
    ctx.cpp<<"namespace InertiaTarget"<<'{'<<std::endl;
    ctx.hpp<<"namespace InertiaTarget"<<'{'<<std::endl;


    ctx.cpp<<"namespace Inertia"<<ctx.inp.target<<'{'<<std::endl;
    ctx.hpp<<"namespace Inertia"<<ctx.inp.target<<'{'<<std::endl;
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
    for(const RegisterEntry& regen : ctx.inp.registers){
        ctx.hpp<<'\t'<<'\t'<<regen.name<<" = new Register_"<<regen.name<<"();"<<std::endl;
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
    
    ctx.hpp<<'\t'<<'~'<<ctx.structName<<"() override"<<'{'<<std::endl;
    
    for(const RegisterEntry& regen : ctx.inp.registers){
        ctx.hpp<<"\t\tdelete "<<regen.name<<';'<<std::endl;
    }
    
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
        char buff[128];

        snprintf(buff, 128, "%d", regen.classid);

        ctx.hpp<<buff<<", ";

        snprintf(buff, 128, "%d", regen.width);

        ctx.hpp<<buff<<')'<<'{';

        if(!cons){
            ctx.hpp<<'}'<<std::endl;
        }
        else{
            ctx.hpp<<std::endl;

            for(const std::string& sw : regen.init){
                ctx.hpp<<'\t'<<'\t'<<sw<<std::endl;
            }

            for(size_t i : regen.dataIndeces){
                InitData(ctx, ctx.inp.datas[i], 2);
            }

            for(const DataInitEntry& din : regen.inits){
                if(!din.init){
                    continue;
                }
                InitData(ctx, ctx.inp.datas[din.di], 2, true, din.val);
            }

            ctx.hpp<<'\t'<<'}'<<std::endl;
        }

        ctx.hpp<<"};"<<std::endl;
    }

    return false;
}

bool TargetCodegen::output(){
    if(!input.file){
        return true;
    }
    if(input.target.empty()) return true;

    std::filesystem::path fullpath = input.file.get_path();

    std::filesystem::path headerpath = fullpath;
    headerpath += ".hpp";

    std::filesystem::path cpppath = fullpath;
    cpppath += ".cpp";

    TargetCodegenCTX ctx(input, cpppath, headerpath);

    if(!ctx.hpp){
        return true;
    }

    if(!ctx.cpp){
        return true;
    }

    GenerateHeaderGuardsTop(ctx);

    WriteLineComment(ctx.hpp, "This file is auto generated by Inertia");

    // writes cppinc
    WriteIncludes(ctx);
    // writes internal includes
    WriteInclude(ctx.cpp, headerpath, true);

    WriteLineComment(ctx.cpp, "This file is auto generated by Inertia");

    ctx.cpp<<'\n';
    ctx.hpp<<'\n';
    
    WriteInclude(ctx.hpp, "Inertia/Target/TargetBase.hpp");
    
    StartFileGen(ctx);

    WriteRegisterClasses(ctx);
    WriteRegisters(ctx);
    WriteBaseClass(ctx);

    // body

    FinalizeFileGen(ctx);

    ctx.hpp<<'\n';
    ctx.cpp<<'\n';

    GenerateHeaderGuardsBottom(ctx);

    return false;
}

}