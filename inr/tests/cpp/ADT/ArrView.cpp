// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/ADT/ArrView.h>

int arr[4] = {1, 2, 3, 4};

int main() {
    inr::arrview<int> arr_view(arr);

    if(arr_view.data() != arr) return 1;

    if(arr_view.size() != 4) return 1;

    inr::arrview<inr::arrview<int>> arr_view_view(&arr_view, 1);

    if(arr_view_view.data()->data() != arr) return 1;

    if(arr_view.find_bool(4) != true) return 1;
    if(arr_view.find_bool(0) == true) return 1;

    return 0;
}
