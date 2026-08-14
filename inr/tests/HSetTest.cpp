// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/ADT/HSet.h>
#include <inr/Support/Stream.h>

#include <cstddef>

int integerSetTest() {
    inr::HSet<int> set;

    auto [v, e] = set.try_emplace(190);
    if(!e) return 1;
    if(*v != 190) return 1;

    auto [v2, e2] = set.try_emplace(190);
    if(e2) return 1;
    if(*v2 != 190) return 1;

    set.clear();

    for(std::size_t i = 0; i < 0x100000; i++) {
        auto p = set.try_emplace(i);
        if(!p.second) {
            inr::err() << "Couldn't emplace at index: " << i << '\n';
            return 1;
        }
    }

    for(std::size_t i = 0; i < 0x100000; i++) {
        auto p = set.try_emplace(i);
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
        auto v = set.find(i);
        if(!v) {
            inr::err() << "Index should be present: " << i << '\n';
            return 1;
        }
    }

    for(std::size_t i = 0x0F0000; i < 0x100000; i++) {
        bool e = set.erase(i);
        if(!e) {
            inr::err() << "Index should be erased: " << i << '\n';
            return 1;
        }
    }

    for(std::size_t i = 0x0F0000; i < 0x100000; i++) {
        auto v = set.find(i);
        if(v) {
            inr::err() << "Index shouldn't be found: " << i << '\n';
            return 1;
        }
    }

    return 0;
}

int main() {
    if(int res = integerSetTest()) return res;

    return 0;
}
