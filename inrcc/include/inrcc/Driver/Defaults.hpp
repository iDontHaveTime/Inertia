#ifndef INRCC_DEFAULTS_HPP
#define INRCC_DEFAULTS_HPP

#include "Inertia/Target/Triple.hpp"
#include "inrcc/Driver/Linker.hpp"

namespace inrcc{
namespace defaults{
extern const inrcc::Linker& default_linker;
extern const Inertia::TargetTriple default_triple;
}
}

#endif // INRCC_DEFAULTS_HPP
