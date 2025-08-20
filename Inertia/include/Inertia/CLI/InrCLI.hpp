#ifndef INERTIA_INRCLI_HPP
#define INERTIA_INRCLI_HPP

#include <functional>
#include <string_view>
#include <unordered_map>

namespace Inertia{
    enum class InrCLIArg{
        None,
        Print,
        Exec,
    };

    struct CLIFlag{
        InrCLIArg type;

        CLIFlag() noexcept : type(InrCLIArg::None){};
        CLIFlag(InrCLIArg _type) noexcept : type(_type){};

        virtual ~CLIFlag() noexcept = default;
    };

    struct CLIFlagPrintStr : public CLIFlag{
        std::string_view output;

        CLIFlagPrintStr(const std::string_view& _out) noexcept : CLIFlag(InrCLIArg::Print), output(_out){};
    };

    struct CLIFlagExec : public CLIFlag{
        std::function<void(void)> exec;

        CLIFlagExec(const std::function<void(void)>& _exec) noexcept : CLIFlag(InrCLIArg::Exec), exec(_exec){};
    };

    class InrCLI{
        std::unordered_map<std::string_view, CLIFlag*> flags;
    public:

        InrCLI() = default;

        void add_print(const std::string_view& name, const std::string_view& output){
            flags[name] = new CLIFlagPrintStr(output);
        }

        void add_exec(const std::string_view& name, const std::function<void(void)>& func){
            flags[name] = new CLIFlagExec(func);
        }

        bool parse_args(int argc, char** argv);

        ~InrCLI() noexcept{
            for(auto& ptr : flags){
                delete ptr.second;
            }
        }
    };
}

#endif // INERTIA_INRCLI_HPP
