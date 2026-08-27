// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/ADT/IVector.h>

#include <string>

int main() {
    inr::ivec<int, 4> int_vec;
    int_vec.emplace(int_vec.end(), 0);

    if(int_vec.size() != 1) return 1;
    if(int_vec[0] != 0) return 1;

    int_vec.erase(int_vec.begin());
    if(int_vec.size() != 0) return 1;

    int_vec.insert(int_vec.begin(), 1);
    int_vec.emplace(int_vec.begin(), 0);

    if(int_vec.find(0) != int_vec.find(1) - 1) return 1;

    for(int x : {2, 3, 4, 5, 6}) {
        int_vec.push_back(x);
    }

    std::size_t size = int_vec.size();

    int_vec.pop_back();
    if(int_vec.size() != size - 1) return 1;

    int_vec.clear();
    if(int_vec.size() != 0) return 1;

    inr::ivec<std::string, 4> string_vec;
    string_vec.emplace_back("string0");

    if(string_vec.size() != 1) return 1;

    for(const char* str : {"string1", "string2", "string3", "string4"}) {
        string_vec.emplace_back(str);
    }

    if(string_vec.size() != 5) return 1;

    return 0;
}
