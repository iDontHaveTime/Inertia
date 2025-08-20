#include "inrcc/Driver/Defaults.hpp"

namespace inrcc{
namespace defaults{
const inrcc::Linker& default_linker = inrcc::linkers::ld_linker;
const Inertia::TargetTriple default_triple = {"x86_64-linux-gnu"};
}
}
