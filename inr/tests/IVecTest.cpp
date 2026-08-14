// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/ADT/IVector.h>
#include <inr/Support/Stream.h>

#include <string>

int main() {
    inr::ivec<int, 4> on_stack = {1, 2, 3, 4};

    if(on_stack[0] != 1) return 1;
    if(on_stack[1] != 2) return 1;
    if(on_stack[2] != 3) return 1;
    if(on_stack[3] != 4) return 1;

    on_stack.push_back(5);

    if(on_stack[0] != 1) return 1;
    if(on_stack[1] != 2) return 1;
    if(on_stack[2] != 3) return 1;
    if(on_stack[3] != 4) return 1;
    if(on_stack[4] != 5) return 1;

    inr::ivec<int, 4> on_heap(on_stack);

    if(on_heap[0] != 1) return 1;
    if(on_heap[1] != 2) return 1;
    if(on_heap[2] != 3) return 1;
    if(on_heap[3] != 4) return 1;
    if(on_heap[4] != 5) return 1;

    if(on_heap.size() != on_stack.size()) return 1;

    if(on_heap.pop_back(), on_heap.size() == on_stack.size()) return 1;

    inr::ivec<std::string, 4> more_complex;

    more_complex = {"string1", "string2", "string3", "string4"};

    for(auto it = more_complex.rbegin(); it != more_complex.rend(); ++it) {
        inr::out() << *it << '\n';
    }

    for(const std::string& str : more_complex) {
        inr::out() << str << '\n';
    }

    more_complex.push_back("string5");

    on_heap.push_back(int(more_complex.front()[0]));

    if(!(on_heap.size() == on_stack.size() &&
         on_heap.size() == more_complex.size()))
        return 1;

    more_complex.erase_if_found("string3");

    for(const std::string& str : more_complex) {
        inr::out() << str << '\n';
    }

    return 0;
}
