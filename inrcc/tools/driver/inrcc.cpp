/* ==================================================== */
// THE FRONTEND DRIVER FOR INRCC AKA THE C/C++ FRONTEND
// This is the Inertia C/C++ frontend driver.
// It takes C/C++ source files, parses them, and then translates them to Inertia's IR.
/* ==================================================== */
#include "Inertia/CLI/InrCLI.hpp"
#include "inrcc/Driver/Presets.hpp"

using Inertia::InrCLI;

void preset_default_flags(InrCLI& cli);

int inrcc_main(int argc, char** argv){
    InrCLI cli;

    preset_default_flags(cli);

    cli.parse_args(argc, argv);

    return 0;
}

void preset_default_flags(InrCLI& cli){
    cli.add_exec("--default-linker", inrcc::presets::print_default_linker);
}
