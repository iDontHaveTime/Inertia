// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/ADT/IList.h>
#include <inr/Support/Stream.h>

#include <cstddef>

struct int_node : public inr::ilist_node<int_node> {
    int val;

    int_node(int v) : val(v) {}
};

int main() {
    inr::ilist<int_node> ints;

    for(std::size_t i = 0; i < 0x20; i++) {
        ints.push_back(new int_node(i));
    }

    for(const int_node& n : ints) {
        inr::out() << n.val << '\n';
    }

    for(std::size_t i = 0; i < 0x20; i++) {
        ints.push_front(new int_node(i));
    }

    for(const int_node& n : ints) {
        inr::out() << n.val << '\n';
    }

    ints.deleteNodes();

    return 0;
}
