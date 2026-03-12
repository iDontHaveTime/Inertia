#include <inr/Option/Arg.h>
#include <inr/Option/ArgList.h>
#include <inr/Option/Option.h>
#include <inr/Option/OptionTable.h>

namespace inr::opt {

Option OPT_INPUT(Option::OptionID::Input, nullptr, "<input>", nullptr);

ArgList OptionTable::parseArgs(int argc, char* const* argv,
                               unsigned missingArgsCount) {
    ArgList newList;

    int i = 1;
    while(i < argc) {
        sview arg = sview(argv[i]);

        if(arg[0] == '-') {
        }
        else {
            newList.newArg(arg, &OPT_INPUT, {});
        }
        i++;
    }

    return newList;
}

} // namespace inr::opt