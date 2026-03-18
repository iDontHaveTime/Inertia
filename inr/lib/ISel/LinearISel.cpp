// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/ISel/LinearISel.h>
#include <inr/ISel/x86/LinearISelPass.h>

namespace inr {

std::unique_ptr<ISelPass> TripleLinearISel::get(inr::Triple T) {
    switch(T.getArch()) {
        case Triple::Arch::Unknown:
            return std::unique_ptr<ISelPass>(nullptr);
        case Triple::Arch::x86_64:
            return std::unique_ptr<ISelPass>(new x86::LinearISelPass());
    }
}

} // namespace inr