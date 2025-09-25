#ifndef INERTIA_INRCLI_HPP
#define INERTIA_INRCLI_HPP

#include <functional>
#include <string_view>
#include <unordered_map>

/* Inertia CLI Handler.
 * This class simplifies the handling of argc and argv by providing actions after a flag that has been defined.
*/

namespace Inertia{
    /* The type of the flag the string represents. */
    enum class InrCLIArg{
        None,
        Print,
        Exec,
    };

    /* Base class for all the different CLI Flag types. */
    struct CLIFlag{
        InrCLIArg type;

        CLIFlag() noexcept : type(InrCLIArg::None){};
        CLIFlag(InrCLIArg _type) noexcept : type(_type){};

        virtual ~CLIFlag() noexcept = default;
    };

    /* Prints out the string inside the class to stdout. */
    struct CLIFlagPrintStr : public CLIFlag{
        std::string_view output;

        CLIFlagPrintStr(const std::string_view& _out) noexcept : CLIFlag(InrCLIArg::Print), output(_out){};
    };

    /* Executes the function provided inside, must be with the void(void) signature. */
    struct CLIFlagExec : public CLIFlag{
        std::function<void(void)> exec;

        CLIFlagExec(const std::function<void(void)>& _exec) noexcept : CLIFlag(InrCLIArg::Exec), exec(_exec){};
    };

    /* The class that handles the CLI for you. */
    class InrCLI{
        /* Stores the flags in a map with the corresponding string. */
        std::unordered_map<std::string_view, CLIFlag*> flags;
    public:

        /* The default constructor. */
        InrCLI() = default;

        /* Adds a print flag to the flag map. */
        void add_print(const std::string_view& name, const std::string_view& output){
            flags[name] = new CLIFlagPrintStr(output);
        }

        /* Adds a function flag to the flag map. */
        void add_exec(const std::string_view& name, const std::function<void(void)>& func){
            flags[name] = new CLIFlagExec(func);
        }

        /* It searches for flags from the argv provided then executes them. */
        bool parse_args(int argc, char** argv);

        /* The destructor just frees all the flags and then the map. */
        ~InrCLI() noexcept{
            for(auto& ptr : flags){
                delete ptr.second;
            }
        }
    };
}

#endif // INERTIA_INRCLI_HPP
