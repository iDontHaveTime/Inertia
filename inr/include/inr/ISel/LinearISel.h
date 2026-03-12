#ifndef INERTIA_ISEL_LINEARISEL_H
#define INERTIA_ISEL_LINEARISEL_H

/// @file ISel/LinearISel.
/// @brief Contains the triple based linear ISel class.

#include <inr/PassManager.h>
#include <inr/Target/Triple.h>

namespace inr {

class TripleLinearISel {
public:
    static std::unique_ptr<inr::ISelPass> get(inr::Triple);
};

} // namespace inr

#endif // INERTIA_ISEL_LINEARISEL_H
