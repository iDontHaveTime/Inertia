// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/ADT/HMap.h>
#include <inr/Support/Stream.h>

#include <cstddef>
#include <string_view>

int integerMapTest() {
    inr::HMap<int, int> map;

    auto [v, e] = map.try_emplace(190, 42);
    if(!e) return 1;
    if(*v != 42) return 1;

    auto [v2, e2] = map.try_emplace(190, 172);
    if(e2) return 1;
    if(*v2 != 42) return 1;

    map.clear();

    for(std::size_t i = 0; i < 0x100000; i++) {
        auto p = map.try_emplace(i, i);
        if(!p.second) {
            inr::err() << "Couldn't emplace at index: " << i << '\n';
            return 1;
        }
    }

    for(std::size_t i = 0; i < 0x100000; i++) {
        auto p = map.try_emplace(i, i);
        if(p.second) {
            inr::err() << "Element should already exist at index: " << i
                       << '\n';
            return 1;
        }
        if(*p.first != i) {
            inr::err() << "Element should equal index at index: " << i << '\n';
            return 1;
        }
    }

    for(std::size_t i = 0; i < 0x100000; i++) {
        auto v = map.find(i);
        if(!v) {
            inr::err() << "Index should be present: " << i << '\n';
            return 1;
        }
    }

    for(std::size_t i = 0x0F0000; i < 0x100000; i++) {
        bool e = map.erase(i);
        if(!e) {
            inr::err() << "Index should be erased: " << i << '\n';
            return 1;
        }
    }

    for(std::size_t i = 0x0F0000; i < 0x100000; i++) {
        auto v = map.find(i);
        if(v) {
            inr::err() << "Index shouldn't be found: " << i << '\n';
            return 1;
        }
    }

    return 0;
}

int stringMapTest() {
    inr::HMap<std::string_view, unsigned> map;

    auto p = map.try_emplace("42", 42);

    if(auto v = map.find("42")) {
        if(*v != 42) return 1;
    }
    else return 1;

    return 0;
}

int main() {
    if(int res = integerMapTest()) return res;
    if(int res = stringMapTest()) return res;

    return 0;
}
