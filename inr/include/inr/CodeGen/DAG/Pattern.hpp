#ifndef INERTIA_PATTERN_HPP
#define INERTIA_PATTERN_HPP

/**
 * @file inr/CodeGen/DAG/Pattern.hpp
 * @brief Header for the pattern class.
 *
 * This header contains the Inertia's pattern class.
 *
 **/

#include "inr/CodeGen/DAG/DAGNode.hpp"

namespace inr::dag{
    struct DAGPatternNode{
        DAGOpcode op;
    };
}

#endif // INERTIA_PATTERN_HPP
