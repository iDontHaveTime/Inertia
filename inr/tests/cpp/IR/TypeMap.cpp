// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/IR/Type.h>
#include <inr/IR/TypeMap.h>

#include <cstddef>

bool testInteger(inr::TypeMap& map) {
    constexpr std::size_t integer_test_size = 0x3000;
    const inr::IntType* int_types[integer_test_size];

    for(unsigned i = 0; i < integer_test_size; i++) {
        int_types[i] = map.getInt(i);
    }

    for(unsigned i = 0; i < integer_test_size; i++) {
        if(int_types[i] != map.getInt(i)) {
            return false;
        }
    }

    return true;
}

bool testFunction(inr::TypeMap& map) {
    constexpr std::size_t function_test_size = 0x3000;

    const inr::FuncType* func_types[0x3000];

    func_types[0] = map.getFunc(map.getVoid(), {}, false);
    func_types[1] = map.getFunc(map.getVoid(), {}, true);

    for(unsigned i = 2; i < function_test_size; i++) {
        func_types[i] = map.getFunc(
            map.getInt(i),
            {map.getInt(i - 1), map.getPtr(), map.getInt(i + 500)}, i & 1);
    }

    if(func_types[0] != map.getFunc(map.getVoid(), {}, false)) return false;
    if(func_types[1] != map.getFunc(map.getVoid(), {}, true)) return false;

    for(unsigned i = 2; i < function_test_size; i++) {
        if(func_types[i] !=
           map.getFunc(map.getInt(i),
                       {map.getInt(i - 1), map.getPtr(), map.getInt(i + 500)},
                       i & 1))
            return false;
    }

    return true;
}

bool testArray(inr::TypeMap& map) {
    constexpr std::size_t array_test_size = 0x3000;

    const inr::ArrayType* array_types[array_test_size];

    for(unsigned i = 0; i < array_test_size; i++) {
        array_types[i] =
            map.getArray(((i & 1) ? map.getInt(i + 1)->as<inr::Type>()
                                  : map.getPtr()->as<inr::Type>()),
                         i + 1);
    }

    for(unsigned i = 0; i < array_test_size; i++) {
        if(array_types[i] !=
           map.getArray(((i & 1) ? map.getInt(i + 1)->as<inr::Type>()
                                 : map.getPtr()->as<inr::Type>()),
                        i + 1))
            return false;
    }

    return true;
}

int main() {
    inr::TypeMap map;

    if(!testInteger(map)) return 1;
    if(!testFunction(map)) return 1;
    if(!testArray(map)) return 1;

    return 0;
}
