#ifndef INERTIA_X86_TARGETTREE_H
#define INERTIA_X86_TARGETTREE_H

/// @file ISel/x86/TargetTree.h
/// @brief Contains the x86 target tree get function.

#include <inr/ISel/TargetTree.h>

namespace inr::x86 {

/// @brief Follows the src-dest pattern.
enum class Operand : OperandType {
    R8,  ///< 8bit Register
    R16, ///< 16bit Register
    R32, ///< 32bit Register
    R64  ///< 64bit Register
};

arrview<TreeNodeObjectFunc> getTargetTree();

} // namespace inr::x86

#endif // INERTIA_X86_TARGETTREE_H
