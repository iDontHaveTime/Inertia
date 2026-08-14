// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/IR/TypeMap.h>

int main() {
    inr::TypeMap tm;

    if(tm.getI32() != tm.getI32()) return 1;
    if(tm.getI64() != tm.getInt(64)) return 1;

    if(tm.getInt(42) != tm.getInt(42)) return 1;

    auto mt = tm.getFunc(tm.getI32(), {tm.getI32(), tm.getPtr()}, false);

    auto mt2 = tm.getFunc(tm.getI32(), {tm.getI32(), tm.getPtr()}, false);

    if(mt != mt2) return 1;

    return 0;
}
