// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/Support/Logger.h>
#include <inr/Support/Stream.h>

/// @brief This file is for testing the logger class.

int main() {
    inr::log::send(inr::errs(), inr::log::Level::ERROR, "inertia",
                   "test error");

    inr::log::send(inr::errs(), inr::log::Level::WARN, inr::log::none(),
                   "test warning, no author");

    inr::log::send(inr::errs(), inr::log::Level::NONE, "inertia",
                   "test nothing");

    inr::log::sendpos(inr::errs(), inr::log::Level::ERROR, "main.cpp", 3, 20,
                      "unexpected token");

    return 0;
}