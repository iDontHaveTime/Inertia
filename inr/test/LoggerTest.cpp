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